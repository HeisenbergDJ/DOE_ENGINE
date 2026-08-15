/********************************************************************
 * "run_context.h" declares the global run directory context.      *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#ifndef DOE_ENGINE_UTILS_TOOLS_RUN_CONTEXT_H_
#define DOE_ENGINE_UTILS_TOOLS_RUN_CONTEXT_H_

#include <filesystem>

namespace doe::utils {

// Global run directory context for one DOE Engine run. A run is anchored at the
// directory of experiment.json; the three directories are treated as
// system-global for the duration of the run so they do not have to be threaded
// through every interface. The Experiment coordinator initializes the context
// once per run and updates design_point_dir before each design point.
class RunContext {
public:
    static RunContext& Instance();

    void Initialize(std::filesystem::path experiment_dir, std::filesystem::path run_dir,
                    std::filesystem::path design_point_dir);
    void Reset();

    const std::filesystem::path& experiment_dir() const noexcept;
    const std::filesystem::path& run_dir() const noexcept;
    const std::filesystem::path& design_point_dir() const noexcept;

private:
    RunContext() = default;

    std::filesystem::path experiment_dir_;
    std::filesystem::path run_dir_;
    std::filesystem::path design_point_dir_;
};

}  // namespace doe::utils

#endif  // DOE_ENGINE_UTILS_TOOLS_RUN_CONTEXT_H_
