/********************************************************************
 * "run_context.cc" provides the global run directory context.     *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#include "run_context.h"

#include <utility>

namespace doe::utils {

RunContext& RunContext::Instance()
{
    static RunContext instance;
    return instance;
}

void RunContext::Initialize(std::filesystem::path experiment_dir,
                            std::filesystem::path run_dir,
                            std::filesystem::path design_point_dir)
{
    experiment_dir_ = std::move(experiment_dir);
    run_dir_ = std::move(run_dir);
    design_point_dir_ = std::move(design_point_dir);
}

void RunContext::Reset()
{
    experiment_dir_.clear();
    run_dir_.clear();
    design_point_dir_.clear();
}

const std::filesystem::path& RunContext::experiment_dir() const noexcept
{
    return experiment_dir_;
}

const std::filesystem::path& RunContext::run_dir() const noexcept
{
    return run_dir_;
}

const std::filesystem::path& RunContext::design_point_dir() const noexcept
{
    return design_point_dir_;
}

}  // namespace doe::utils
