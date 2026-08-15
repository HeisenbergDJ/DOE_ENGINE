/********************************************************************
 * "path_template.h" declares path template expansion.            *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#ifndef DOE_ENGINE_UTILS_TOOLS_PATH_TEMPLATE_H_
#define DOE_ENGINE_UTILS_TOOLS_PATH_TEMPLATE_H_

#include <filesystem>
#include <map>
#include <string>

namespace doe::utils {

// Expands {experiment_dir}, {run_dir}, {design_point_dir} and {input:<port>}
// placeholders in a path template. The three run directories come from the
// global RunContext; {input:<port>} is replaced by the bound input file
// recorded in input_files. Unknown or unclosed placeholders throw
// doe::log::RuntimeError.
std::string ExpandPathTemplate(
    const std::string& text,
    const std::map<std::string, std::filesystem::path>& input_files);

}  // namespace doe::utils

#endif  // DOE_ENGINE_UTILS_TOOLS_PATH_TEMPLATE_H_
