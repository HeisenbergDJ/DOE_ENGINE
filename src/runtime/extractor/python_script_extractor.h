/********************************************************************
 * "python_script_extractor.h" declares the python_script type.    *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#ifndef DOE_ENGINE_RUNTIME_EXTRACTOR_PYTHON_SCRIPT_EXTRACTOR_H_
#define DOE_ENGINE_RUNTIME_EXTRACTOR_PYTHON_SCRIPT_EXTRACTOR_H_

#include "extractor.h"

#include <filesystem>
#include <map>
#include <string>
#include <vector>

namespace doe::runtime {

// Runtime form of one python result value binding.
struct PythonResultValue {
    std::string key;
    std::string output;
    doe::config::ValueType value_type;
};

// python_script: runs the interpreter on the script, then reads the JSON result
// file and binds each declared key to its output-port response.
class PythonScriptExtractor : public ResponseExtractor {
public:
    PythonScriptExtractor(std::string interpreter, std::string script,
                          std::string working_directory, std::vector<std::string> arguments,
                          long long timeout_seconds, std::vector<int> success_exit_codes,
                          std::string result_file, std::vector<PythonResultValue> result_values);

    std::map<std::string, ScalarValue> Extract(
        const std::string& instance_id,
        const std::map<std::string, std::string>& output_bindings,
        const std::map<std::string, std::filesystem::path>& input_files) const override;

private:
    std::string interpreter_;
    std::string script_;
    std::string working_directory_;
    std::vector<std::string> arguments_;
    long long timeout_seconds_;
    std::vector<int> success_exit_codes_;
    std::string result_file_;
    std::vector<PythonResultValue> result_values_;
};

}  // namespace doe::runtime

#endif  // DOE_ENGINE_RUNTIME_EXTRACTOR_PYTHON_SCRIPT_EXTRACTOR_H_
