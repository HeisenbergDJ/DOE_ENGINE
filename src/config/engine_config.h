/********************************************************************
 * "engine_config.h" defines the engine run-policy configuration.  *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-16                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#ifndef DOE_ENGINE_CONFIG_ENGINE_CONFIG_H_
#define DOE_ENGINE_CONFIG_ENGINE_CONFIG_H_

#include "config_types.h"

#include <cstddef>
#include <filesystem>
#include <string>

namespace doe::config {

// Serialized by engine_cfg.json, the startup configuration of the doe_engine
// executable. It holds run policy only (persistence, filesystem layout,
// logging, scheduling, limits); the scientific definition stays in
// experiment.json and never appears here.
struct PersistenceConfig {
    std::filesystem::path db_path;             // empty = {experiment_dir}/doe_engine.db
    std::size_t batch_size = 256;
    ArtifactStrategy artifact_strategy = ArtifactStrategy::kKeepAll;
    std::size_t tar_batch_size = 10000;        // points per tar batch when strategy is tar
};

struct FileSystemConfig {
    WorkingDirectoryMode working_directory = WorkingDirectoryMode::kShared;
    std::filesystem::path scratch_dir;         // required when working_directory is scratch
};

struct LoggingConfig {
    LogLevel level = LogLevel::kInfo;
    bool to_stdout = false;
    std::string file = "{run_dir}/engine.log";
};

struct SchedulerConfig {
    SchedulerMode mode = SchedulerMode::kStatic;
    std::size_t chunk_size = 0;                // 0 = auto
};

struct LimitsConfig {
    std::size_t max_design_points = 0;         // 0 = unlimited
};

struct EngineConfig {
    PersistenceConfig persistence;
    FileSystemConfig filesystem;
    LoggingConfig logging;
    SchedulerConfig scheduler;
    LimitsConfig limits;
};

}  // namespace doe::config

#endif  // DOE_ENGINE_CONFIG_ENGINE_CONFIG_H_
