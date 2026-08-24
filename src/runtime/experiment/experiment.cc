/********************************************************************
 * "experiment.cc" provides the Experiment orchestrator.           *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#include "experiment.h"

#include "application.h"
#include "error.h"
#include "json.hpp"
#include "run_context.h"
#include "scalar_value.h"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

namespace doe::runtime {

namespace {

using doe::log::RuntimeError;

// How many finished design point records accumulate before a non-root rank
// ships them to the root, and how many records each Persistence transaction
// commits at once.
constexpr std::size_t kRecordBatchSize = 256;

nlohmann::json ScalarToJson(const doe::runtime::ScalarValue& value)
{
    return std::visit([](const auto& alternative) -> nlohmann::json { return alternative; },
                      value);
}

doe::runtime::ScalarValue JsonToScalar(const nlohmann::json& value)
{
    if (value.is_number_integer()) {
        return value.get<int>();
    }
    if (value.is_number_float()) {
        return value.get<double>();
    }
    return value.get<std::string>();
}

std::map<std::string, doe::runtime::ScalarValue> JsonToScalarMap(const nlohmann::json& object)
{
    std::map<std::string, doe::runtime::ScalarValue> result;
    for (const auto& entry : object.items()) {
        result.emplace(entry.key(), JsonToScalar(entry.value()));
    }
    return result;
}

void EnsureDirectory(const std::filesystem::path& path, const std::string& stage,
                     const std::string& object_id)
{
    std::error_code error;
    std::filesystem::create_directories(path, error);
    if (error) {
        throw RuntimeError(stage, object_id, path,
                           "failed to create directory: " + error.message());
    }
}

}  // namespace

Experiment::Experiment(doe::config::LoadedExperimentConfig config,
                       doe::persistence::Persistence* persistence)
    : config_hash_(std::move(config.config_hash)), persistence_(persistence)
{
    for (const auto& response : config.responses.responses) {
        responses_.emplace(response.id, Response(response));
    }

    std::map<std::string, Application> applications;
    for (auto& entry : config.applications) {
        applications.emplace(entry.first, Application(std::move(entry.second)));
    }

    sampler_ = MakeSampler(config.variables, config.sampling);
    workflow_runner_ = MakeWorkflowRunner(config.workflow, std::move(applications));
    extractor_runner_ = MakeExtractorRunner(config.extractors, config.extractor_definitions);
    scheduler_ = MakeScheduler();

    doe::utils::RunContext::Instance().Initialize(config.experiment_file.parent_path(), {},
                                                  {});
}

std::string Experiment::CreateRunId() const
{
    const std::time_t now = std::time(nullptr);
    std::tm local = {};
#ifdef _WIN32
    localtime_s(&local, &now);
#else
    localtime_r(&now, &local);
#endif
    char buffer[32] = {};
    std::strftime(buffer, sizeof buffer, "run_%Y%m%d_%H%M%S", &local);
    return std::string(buffer);
}

ExperimentResult Experiment::Run(const std::optional<std::string>& resume_run_id)
{
    const std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
    doe::utils::RunContext& run_context = doe::utils::RunContext::Instance();
    const std::string run_id = scheduler_->BeginRun(resume_run_id.value_or(CreateRunId()));
    const std::filesystem::path run_dir = run_context.experiment_dir() / "runs" / run_id;
    EnsureDirectory(run_dir, "run", run_id);

    std::ofstream run_log(run_dir / "engine.log", std::ios::app);
    doe::log::Logger logger(run_log, scheduler_->Rank());
    logger.Info("run started (run_id = " + run_id + ")");

    ExperimentResult result;
    result.run_id = run_id;

    std::vector<std::size_t> terminal_points;
    if (resume_run_id) {
        // Only the root holds a Persistence, so only it can validate the resume
        // request; the scheduler then shares the decision with every rank.
        ResumeInfo local_info;
        if (persistence_) {
            const std::string recorded_hash = persistence_->ConfigHash(run_id);
            if (recorded_hash.empty()) {
                local_info.abort = true;
                local_info.abort_message = "no such run in the database";
            } else if (recorded_hash != config_hash_) {
                local_info.abort = true;
                local_info.abort_message =
                    "experiment configuration changed; refusing to resume";
            } else {
                local_info.terminal_points = persistence_->TerminalPoints(run_id);
                std::sort(local_info.terminal_points.begin(), local_info.terminal_points.end());
                logger.Info("resuming run: " + std::to_string(local_info.terminal_points.size()) +
                            " design points already completed");
            }
        }
        const ResumeInfo info = scheduler_->BroadcastResumeInfo(local_info);
        if (info.abort) {
            throw RuntimeError("resume", run_id, std::filesystem::path(), info.abort_message);
        }
        terminal_points = std::move(info.terminal_points);
    }

    if (persistence_ && scheduler_->IsRoot()) {
        persistence_->RecordExperimentStart(run_id, config_hash_);
    }

    const std::size_t point_count = sampler_->Size();
    points_.clear();
    RunSummary summary;
    std::vector<std::string> pending_records;
    try {
        summary = scheduler_->Run(
            point_count,
            [this, run_id, run_dir, terminal_points, &logger, point_count,
             &pending_records](std::size_t index) {
                bool ok = true;
                if (std::binary_search(terminal_points.begin(), terminal_points.end(), index)) {
                    logger.Info("design point " + std::to_string(index) + "/" +
                                std::to_string(point_count) + ": skipped (already completed)");
                } else {
                    ok = RunPoint(index, run_id, run_dir, logger, &pending_records);
                    logger.Info("design point " + std::to_string(index) + "/" +
                                std::to_string(point_count) + ": " + (ok ? "ok" : "failed"));
                }
                if (scheduler_->IsRoot()) {
                    // The root persists whatever batches have already arrived,
                    // so database writes overlap with computation.
                    scheduler_->DrainAvailable(
                        [this, run_id](const std::vector<std::string>& batch) {
                            PersistBatch(batch, run_id);
                        });
                } else if (pending_records.size() >= kRecordBatchSize) {
                    scheduler_->SendBatch(pending_records);
                    pending_records.clear();
                }
                return ok;
            });
    } catch (const std::exception& error) {
        logger.Error(std::string("run aborted: ") + error.what());
        if (persistence_ && scheduler_->IsRoot()) {
            persistence_->RecordExperimentEnd(run_id, false);
        }
        run_context.Reset();
        throw;
    }

    if (scheduler_->IsRoot()) {
        // Persist this rank's own records, then drain everything still in
        // flight from the other ranks.
        for (std::size_t offset = 0; offset < pending_records.size(); offset += kRecordBatchSize) {
            const std::size_t end =
                std::min(offset + kRecordBatchSize, pending_records.size());
            PersistBatch(std::vector<std::string>(pending_records.begin() +
                                                      static_cast<std::ptrdiff_t>(offset),
                                                  pending_records.begin() +
                                                      static_cast<std::ptrdiff_t>(end)),
                         run_id);
        }
        scheduler_->DrainAll([this, run_id](const std::vector<std::string>& batch) {
            PersistBatch(batch, run_id);
        });
    } else {
        scheduler_->SendBatch(pending_records);
        scheduler_->FinishSending();
    }

    result.point_count = summary.point_count;
    result.success = summary.success_count == summary.point_count;
    const double elapsed_seconds =
        std::chrono::duration<double>(std::chrono::steady_clock::now() - start_time).count();
    logger.Info("run finished: " + std::to_string(summary.success_count) + "/" +
                std::to_string(summary.point_count) + " succeeded in " +
                std::to_string(elapsed_seconds) + " s");
    if (persistence_ && scheduler_->IsRoot()) {
        persistence_->RecordExperimentEnd(run_id, result.success);
    }
    run_context.Reset();
    return result;
}

bool Experiment::RunPoint(std::size_t index, const std::string& run_id,
                          const std::filesystem::path& run_dir, const doe::log::Logger& logger,
                          std::vector<std::string>* records)
{
    doe::utils::RunContext& run_context = doe::utils::RunContext::Instance();
    const std::filesystem::path design_point_dir =
        run_dir / ("DESIGN_POINT_" + std::to_string(index));
    EnsureDirectory(design_point_dir, "design_point", std::to_string(index));
    run_context.Initialize(run_context.experiment_dir(), run_dir, design_point_dir);

    DesignPoint point = sampler_->At(index);

    std::map<std::string, ScalarValue> variable_values;
    std::map<std::string, std::map<std::string, std::filesystem::path>> outputs;
    std::map<std::string, ScalarValue> response_values;
    std::string error_message;
    bool success = false;
    try {
        for (const auto& variable : point.variables()) {
            variable_values.emplace(variable.first, variable.second.value());
        }

        outputs = workflow_runner_->Run(variable_values);

        response_values = extractor_runner_->Run(outputs);
        for (const auto& entry : response_values) {
            const auto definition = responses_.find(entry.first);
            if (definition == responses_.end()) {
                throw RuntimeError("response", entry.first, std::filesystem::path(),
                                   "extractor produced an undeclared response id");
            }
            Response response = definition->second;
            response.set_value(entry.second);
            point.set_response(std::move(response));
        }

        point.set_success(true);
        success = true;
    } catch (const std::exception& error) {
        point.set_success(false);
        point.set_error(error.what());
        error_message = error.what();
        success = false;
        logger.Error("design point " + std::to_string(index) + " failed: " + error_message);
    }

    // Serialize the finished point so the root process can persist it.
    nlohmann::json record;
    record["index"] = index;
    record["success"] = success;
    record["error"] = error_message;
    record["variables"] = nlohmann::json::object();
    for (const auto& entry : variable_values) {
        record["variables"][entry.first] = ScalarToJson(entry.second);
    }
    record["responses"] = nlohmann::json::object();
    for (const auto& entry : response_values) {
        record["responses"][entry.first] = ScalarToJson(entry.second);
    }
    record["applications"] = nlohmann::json::array();
    for (const auto& app_outputs : outputs) {
        nlohmann::json application;
        application["id"] = app_outputs.first;
        application["outputs"] = nlohmann::json::object();
        for (const auto& output : app_outputs.second) {
            application["outputs"][output.first] = output.second.string();
        }
        record["applications"].push_back(std::move(application));
    }
    records->push_back(record.dump());

    points_.push_back(std::move(point));
    return success;
}

void Experiment::PersistRecord(const std::string& record_json, const std::string& run_id)
{
    const nlohmann::json record = nlohmann::json::parse(record_json);
    const std::size_t index = record["index"].get<std::size_t>();
    const bool success = record["success"].get<bool>();
    const std::string error_message = record["error"].get<std::string>();
    const std::map<std::string, ScalarValue> variables = JsonToScalarMap(record["variables"]);
    const std::map<std::string, ScalarValue> responses = JsonToScalarMap(record["responses"]);

    for (const auto& application : record["applications"]) {
        const std::string application_id = application["id"].get<std::string>();
        std::map<std::string, std::filesystem::path> outputs;
        for (const auto& output : application["outputs"].items()) {
            outputs.emplace(output.key(), std::filesystem::path(output.value().get<std::string>()));
        }
        persistence_->RecordApplicationRun(run_id, index, application_id, outputs);
    }
    persistence_->RecordPointOutcome(run_id, index, variables, success, error_message, responses);
}

void Experiment::PersistBatch(const std::vector<std::string>& records, const std::string& run_id)
{
    if (records.empty() || persistence_ == nullptr) {
        return;
    }
    persistence_->BeginBatch();
    try {
        for (const std::string& record : records) {
            PersistRecord(record, run_id);
        }
        persistence_->EndBatch();
    } catch (...) {
        persistence_->AbortBatch();
        throw;
    }
}

const std::vector<DesignPoint>& Experiment::points() const noexcept
{
    return points_;
}

}  // namespace doe::runtime
