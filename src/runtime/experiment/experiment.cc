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
#include <ctime>
#include <filesystem>
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
    : persistence_(persistence)
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

ExperimentResult Experiment::Run()
{
    const std::string run_id = CreateRunId();
    doe::utils::RunContext& run_context = doe::utils::RunContext::Instance();
    const std::filesystem::path run_dir = run_context.experiment_dir() / "runs" / run_id;
    EnsureDirectory(run_dir, "run", run_id);

    ExperimentResult result;
    result.run_id = run_id;
    if (persistence_) {
        persistence_->RecordExperimentStart(run_id);
    }

    points_.clear();
    try {
        points_ = sampler_->Generate();
    } catch (const std::exception& error) {
        if (persistence_) {
            persistence_->RecordExperimentEnd(run_id, false);
        }
        run_context.Reset();
        throw;
    }

    result.point_count = points_.size();
    for (std::size_t index = 0; index < points_.size(); ++index) {
        DesignPoint& point = points_[index];

        const std::filesystem::path design_point_dir =
            run_dir / ("DESIGN_POINT_" + std::to_string(index));
        EnsureDirectory(design_point_dir, "design_point", std::to_string(index));
        run_context.Initialize(run_context.experiment_dir(), run_dir, design_point_dir);
        if (persistence_) {
            persistence_->RecordDesignPoint(run_id, point);
        }

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

            const std::map<std::string, ScalarValue> response_values =
                extractor_runner_->Run(outputs);
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
            if (persistence_) {
                persistence_->RecordResponses(run_id, index, response_values);
            }

            point.set_success(true);
        } catch (const std::exception& error) {
            point.set_success(false);
            point.set_error(error.what());
        }
    }

    result.success = std::all_of(points_.begin(), points_.end(),
                                 [](const DesignPoint& point) { return point.success(); });
    if (persistence_) {
        persistence_->RecordExperimentEnd(run_id, result.success);
    }
    run_context.Reset();
    return result;
}

const std::vector<DesignPoint>& Experiment::points() const noexcept
{
    return points_;
}

}  // namespace doe::runtime
