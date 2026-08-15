/********************************************************************
 * "python_script_extractor.cc" provides the python_script type.   *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#include "python_script_extractor.h"

#include "error.h"
#include "extractor_util.h"
#include "json.hpp"
#include "path_template.h"
#include "process_runner.h"

#include <algorithm>
#include <filesystem>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace doe::runtime {

using doe::utils::ExpandPathTemplate;

PythonScriptExtractor::PythonScriptExtractor(std::string interpreter, std::string script,
                                             std::string working_directory,
                                             std::vector<std::string> arguments,
                                             long long timeout_seconds,
                                             std::vector<int> success_exit_codes,
                                             std::string result_file,
                                             std::vector<PythonResultValue> result_values)
    : interpreter_(std::move(interpreter)),
      script_(std::move(script)),
      working_directory_(std::move(working_directory)),
      arguments_(std::move(arguments)),
      timeout_seconds_(timeout_seconds),
      success_exit_codes_(std::move(success_exit_codes)),
      result_file_(std::move(result_file)),
      result_values_(std::move(result_values))
{
}

std::map<std::string, ScalarValue> PythonScriptExtractor::Extract(
    const std::string& instance_id,
    const std::map<std::string, std::string>& output_bindings,
    const std::map<std::string, std::filesystem::path>& input_files) const
{
    const std::string script = ExpandPathTemplate(script_, input_files);
    const std::filesystem::path working_directory =
        std::filesystem::path(ExpandPathTemplate(working_directory_, input_files));

    std::vector<std::string> arguments;
    arguments.reserve(arguments_.size() + 1);
    arguments.push_back(script);
    for (const std::string& argument : arguments_) {
        arguments.push_back(ExpandPathTemplate(argument, input_files));
    }

    Process process(std::filesystem::path(interpreter_), working_directory, arguments,
                    timeout_seconds_);
    const ProcessResult process_result = process.Run();
    if (process_result.timed_out) {
        throw doe::log::RuntimeError("extractor", instance_id, working_directory,
                                     "python extractor timed out after " +
                                         std::to_string(timeout_seconds_) + " seconds");
    }
    const bool accepted =
        std::find(success_exit_codes_.begin(), success_exit_codes_.end(),
                  process_result.exit_code) != success_exit_codes_.end();
    if (!accepted) {
        throw doe::log::RuntimeError("extractor", instance_id, working_directory,
                                     "unexpected exit code " +
                                         std::to_string(process_result.exit_code));
    }

    const std::filesystem::path result_path =
        std::filesystem::path(ExpandPathTemplate(result_file_, input_files));
    const std::string result_text = ReadTextFile(result_path);
    nlohmann::json result_json;
    try {
        result_json = nlohmann::json::parse(result_text);
    } catch (const nlohmann::json::parse_error& error) {
        throw doe::log::RuntimeError("extractor", instance_id, result_path,
                                     std::string("invalid result JSON: ") + error.what());
    }

    std::map<std::string, ScalarValue> responses;
    for (const auto& value : result_values_) {
        const auto binding = output_bindings.find(value.output);
        if (binding == output_bindings.end()) {
            throw doe::log::RuntimeError("extractor", instance_id, result_path,
                                         "output port \"" + value.output +
                                             "\" has no response binding");
        }
        const auto key = result_json.find(value.key);
        if (key == result_json.end()) {
            throw doe::log::RuntimeError("extractor", instance_id, result_path,
                                         "result key \"" + value.key + "\" not found");
        }
        if (value.value_type != doe::config::ValueType::kReal) {
            throw doe::log::RuntimeError("extractor", instance_id, result_path,
                                         "unsupported value_type");
        }
        if (key->is_number()) {
            responses.emplace(binding->second, key->get<double>());
        } else if (key->is_string()) {
            responses.emplace(binding->second,
                              ParseRealValue(key->get<std::string>(), instance_id));
        } else {
            throw doe::log::RuntimeError("extractor", instance_id, result_path,
                                         "result key \"" + value.key + "\" is not a number");
        }
    }
    return responses;
}

}  // namespace doe::runtime
