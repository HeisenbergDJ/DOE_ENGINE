/********************************************************************
 * "main.cc" defines the DOE Engine command-line entry point.       *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#include "config.h"
#include "experiment.h"
#include "parser.h"
#include "point_scheduler.h"
#include "scalar_value.h"
#include "sqlite_persistence.h"

#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <variant>

namespace {

std::string ToString(const doe::runtime::ScalarValue& value)
{
    return std::visit([](const auto& alternative) -> std::string {
        using T = std::decay_t<decltype(alternative)>;
        if constexpr (std::is_same_v<T, double>) {
            return std::to_string(alternative);
        } else if constexpr (std::is_same_v<T, int>) {
            return std::to_string(alternative);
        } else {
            return alternative;
        }
    }, value);
}

// Directory of the running executable, derived from argv[0]; falls back to the
// current working directory when argv[0] carries no path.
std::filesystem::path ExecutableDirectory(const char* argv0)
{
    std::filesystem::path directory = std::filesystem::path(argv0).parent_path();
    if (directory.empty()) {
        directory = std::filesystem::current_path();
    }
    return std::filesystem::absolute(directory);
}

}  // namespace

int main(int argc, char* argv[])
{
    if (argc != 2 && argc != 4) {
        std::cerr << "usage: doe_engine <experiment.json> [--resume <run_id>]" << std::endl;
        return 2;
    }

    try {
        // Startup configuration: engine_cfg.json must exist next to the
        // executable and be valid; otherwise the process exits with an error.
        const std::filesystem::path engine_config_path =
            ExecutableDirectory(argv[0]) / "engine_cfg.json";
        doe::utils::Parser::ParseEngineConfig(engine_config_path);

        const std::filesystem::path experiment_file = std::filesystem::absolute(argv[1]);
        std::optional<std::string> resume_run_id;
        if (argc == 4) {
            if (std::string(argv[2]) != "--resume") {
                std::cerr << "unknown option: " << argv[2] << std::endl;
                return 2;
            }
            resume_run_id = argv[3];
        }

        const doe::config::LoadedExperimentConfig config =
            doe::utils::Parser::LoadExperiment(experiment_file);

        const std::filesystem::path db_path = experiment_file.parent_path() / "doe_engine.db";
        // Only the root process may touch the database; every other rank gets no
        // Persistence and hands its results to the root through the scheduler.
        std::unique_ptr<doe::persistence::SqlitePersistence> persistence;
        if (doe::runtime::IsCoordinatorProcess()) {
            persistence = std::make_unique<doe::persistence::SqlitePersistence>(db_path);
        }
        doe::runtime::Experiment experiment(config, persistence.get());

        const doe::runtime::ExperimentResult result = experiment.Run(resume_run_id);

        std::cout << "run: " << result.run_id << std::endl;
        std::cout << "design points: " << result.point_count << std::endl;
        for (const auto& point : experiment.points()) {
            std::cout << "design_point " << point.index() << ": "
                      << (point.success() ? "ok" : "failed") << std::endl;
            if (!point.success()) {
                std::cout << "  error: " << point.error() << std::endl;
            }
            for (const auto& response : point.responses()) {
                std::cout << "  " << response.first << " = "
                          << ToString(response.second.value()) << std::endl;
            }
        }
        std::cout << "status: " << (result.success ? "success" : "failed") << std::endl;
        return result.success ? 0 : 1;
    } catch (const std::exception& error) {
        std::cerr << "error: " << error.what() << std::endl;
        return 1;
    }
}
