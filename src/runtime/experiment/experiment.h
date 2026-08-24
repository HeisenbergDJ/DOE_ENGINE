/********************************************************************
 * "experiment.h" declares the Experiment orchestrator.            *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#ifndef DOE_ENGINE_RUNTIME_EXPERIMENT_EXPERIMENT_H_
#define DOE_ENGINE_RUNTIME_EXPERIMENT_EXPERIMENT_H_

#include "config.h"
#include "design_point.h"
#include "extractor.h"
#include "logger.h"
#include "persistence.h"
#include "response.h"
#include "sampling.h"
#include "workflow.h"

#include "point_scheduler.h"

#include <cstddef>
#include <filesystem>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace doe::runtime {

// The summary of one experiment run. The design points themselves are owned by
// the Experiment and are read through Experiment::points().
struct ExperimentResult {
    std::string run_id;
    bool success = false;
    std::size_t point_count = 0;
};

// Runs one DOE experiment end to end: generate the design points, create the
// run and design-point directories, run the Workflow and then the Extractors
// for every design point, and hand the observed values to the optional
// Persistence. The constructor is the only place that reads the
// LoadedExperimentConfig: it consumes it to build the runtime Sampler,
// WorkflowRunner and ExtractorRunner, pre-builds the runtime Applications and
// the Response templates, and keeps no Config member. The Experiment owns the
// generated design points and their observed responses for the whole run.
class Experiment {
public:
    Experiment(doe::config::LoadedExperimentConfig config,
               doe::persistence::Persistence* persistence);

    // Runs the experiment. When resume_run_id is given, the engine reuses that
    // run id (instead of generating a new one) and skips design points that
    // already have a terminal outcome recorded in the Persistence.
    ExperimentResult Run(const std::optional<std::string>& resume_run_id = std::nullopt);

    const std::vector<DesignPoint>& points() const noexcept;

private:
    bool RunPoint(std::size_t index, const std::string& run_id,
                  const std::filesystem::path& run_dir, const doe::log::Logger& logger,
                  std::vector<std::string>* records);

    // Replays one serialized design point record into the Persistence. Called
    // on the root process only.
    void PersistRecord(const std::string& record_json, const std::string& run_id);

    // Persists a batch of records inside one Persistence transaction. Called
    // on the root process only.
    void PersistBatch(const std::vector<std::string>& records, const std::string& run_id);

    std::string CreateRunId() const;

    std::string config_hash_;
    std::map<std::string, Response> responses_;

    std::unique_ptr<Sampler> sampler_;
    std::unique_ptr<WorkflowRunner> workflow_runner_;
    std::unique_ptr<ExtractorRunner> extractor_runner_;
    std::unique_ptr<PointScheduler> scheduler_;
    doe::persistence::Persistence* persistence_;

    std::vector<DesignPoint> points_;
};

}  // namespace doe::runtime

#endif  // DOE_ENGINE_RUNTIME_EXPERIMENT_EXPERIMENT_H_
