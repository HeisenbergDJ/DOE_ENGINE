/********************************************************************
 * "path_template.cc" provides path template expansion.           *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#include "path_template.h"

#include "error.h"
#include "run_context.h"

#include <filesystem>
#include <map>
#include <string>

namespace doe::utils {

std::string ExpandPathTemplate(const std::string& text,
                               const std::map<std::string, std::filesystem::path>& input_files)
{
    const RunContext& context = RunContext::Instance();
    std::string result;
    std::size_t position = 0;
    while (position < text.size()) {
        const std::size_t open = text.find('{', position);
        if (open == std::string::npos) {
            result += text.substr(position);
            break;
        }
        const std::size_t close = text.find('}', open + 1);
        if (close == std::string::npos) {
            throw doe::log::RuntimeError("expand", "", std::filesystem::path(),
                                         "unclosed path placeholder in \"" + text + "\"");
        }
        result += text.substr(position, open - position);
        const std::string token = text.substr(open + 1, close - open - 1);
        std::string value;
        if (token == "experiment_dir") {
            value = context.experiment_dir().string();
        } else if (token == "run_dir") {
            value = context.run_dir().string();
        } else if (token == "design_point_dir") {
            value = context.design_point_dir().string();
        } else if (token.compare(0, 6, "input:") == 0) {
            const std::string port = token.substr(6);
            const auto file = input_files.find(port);
            if (file == input_files.end()) {
                throw doe::log::RuntimeError("expand", "", std::filesystem::path(),
                                             "no bound input file for port \"" + port + "\"");
            }
            value = file->second.string();
        } else {
            throw doe::log::RuntimeError("expand", "", std::filesystem::path(),
                                         "unknown path placeholder \"{" + token + "}\"");
        }
        result += value;
        position = close + 1;
    }
    return result;
}

}  // namespace doe::utils
