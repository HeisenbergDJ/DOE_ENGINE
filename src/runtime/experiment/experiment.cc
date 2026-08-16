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
    if (resume_run_id && persistence_) {
        const std::string recorded_hash = persistence_->ConfigHash(run_id);
        if (recorded_hash.empty()) {
            throw RuntimeError("resume", run_id, std::filesystem::path(),
                               "no such run in the database");
        }
        if (recorded_hash != config_hash_) {
            throw RuntimeError("resume", run_id, std::filesystem::path(),
                               "experiment configuration changed; refusing to resume");
        }
        terminal_points = persistence_->TerminalPoints(run_id);
        std::sort(terminal_points.begin(), terminal_points.end());
        logger.Info("resuming run: " + std::to_string(terminal_points.size()) +
                    " design points already completed");
    }

    if (persistence_ && scheduler_->IsRoot()) {
        persistence_->RecordExperimentStart(run_id, config_hash_);
    }

    const std::size_t point_count = sampler_->Size();
    points_.clear();
    RunSummary summary;
    try {
        summary = scheduler_->Run(
            point_count,
            [this, run_id, run_dir, terminal_points, &logger, point_count](std::size_t index) {
                if (std::binary_search(terminal_points.begin(), terminal_points.end(), index)) {
                    logger.Info("design point " + std::to_string(index) + "/" +
                                std::to_string(point_count) + ": skipped (already completed)");
                    return true;
                }
                const bool ok = RunPoint(index, run_id, run_dir, logger);
                logger.Info("design point " + std::to_string(index) + "/" +
                            std::to_string(point_count) + ": " + (ok ? "ok" : "failed"));
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
                          const std::filesystem::path& run_dir, const doe::log::Logger& logger)
{
    doe::utils::RunContext& run_context = doe::utils::RunContext::Instance();
    const std::filesystem::path design_point_dir =
        run_dir / ("DESIGN_POINT_" + std::to_string(index));
    EnsureDirectory(design_point_dir, "design_point", std::to_string(index));
    run_context.Initialize(run_context.experiment_dir(), run_dir, design_point_dir);

    DesignPoint point = sampler_->At(index);

    std::map<std::string, ScalarValue> response_values;
    std::string error_message;
    bool success = false;
    try {
        std::map<std::string, ScalarValue> variable_values;
        for (const auto& variable : point.variables()) {
            variable_values.emplace(variable.first, variable.second.value());
        }

        const std::map<std::string, std::map<std::string, std::filesystem::path>> outputs =
            workflow_runner_->Run(variable_values);
        for (const auto& app_outputs : outputs) {
            if (persistence_) {
                persistence_->RecordApplicationRun(run_id, index, app_outputs.first,
                                                   app_outputs.second);
            }
        }

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

    if (persistence_) {
        persistence_->RecordPointOutcome(run_id, index, point, success, error_message,
                                         response_values);
    }
    points_.push_back(std::move(point));
    return success;
}

const std::vector<DesignPoint>& Experiment::points() const noexcept
{
    return points_;
}

}  // namespace doe::runtime
