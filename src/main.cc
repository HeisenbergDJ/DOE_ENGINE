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
#include "scalar_value.h"

#include <filesystem>
#include <iostream>
#include <memory>
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

}  // namespace

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "usage: doe_engine <experiment.json>" << std::endl;
        return 2;
    }

    try {
        const std::filesystem::path experiment_file = std::filesystem::absolute(argv[1]);
        const doe::config::LoadedExperimentConfig config =
            doe::utils::Parser::LoadExperiment(experiment_file);

        doe::runtime::Experiment experiment(config, nullptr);

        const doe::runtime::ExperimentResult result = experiment.Run();

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
