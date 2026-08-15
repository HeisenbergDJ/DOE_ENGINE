/********************************************************************
 * "application.cc" provides the runtime Application class.        *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#include "application.h"

#include "error.h"
#include "path_template.h"
#include "process_runner.h"
#include "tagger.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace doe::runtime {

namespace {

using doe::log::RuntimeError;
using doe::utils::ExpandPathTemplate;

std::string ReadTextFile(const std::filesystem::path& path)
{
    std::ifstream stream(path, std::ios::binary);
    if (!stream) {
        throw RuntimeError("read", "", path, "failed to open the template file");
    }
    std::ostringstream buffer;
    buffer << stream.rdbuf();
    return buffer.str();
}

void WriteTextFile(const std::filesystem::path& path, const std::string& content)
{
    std::ofstream stream(path, std::ios::binary | std::ios::trunc);
    if (!stream) {
        throw RuntimeError("write", "", path, "failed to open the destination file");
    }
    stream << content;
}

void EnsureParentDirectories(const std::filesystem::path& path)
{
    if (path.parent_path().empty()) {
        return;
    }
    std::error_code error;
    std::filesystem::create_directories(path.parent_path(), error);
    if (error) {
        throw RuntimeError("input", "", path, "failed to create directories: " + error.message());
    }
}

}  // namespace

Application::Application(doe::config::ApplicationConfig config)
    : executable_(std::move(config.executable)),
      working_directory_(std::move(config.working_directory)),
      arguments_(std::move(config.arguments)),
      timeout_seconds_(config.timeout_seconds),
      success_exit_codes_(std::move(config.success_exit_codes))
{
    inputs_.reserve(config.inputs.size());
    for (auto& input : config.inputs) {
        Input plan;
        plan.mode = input.mode;
        plan.source = std::move(input.source);
        plan.destination = std::move(input.destination);
        if (input.mode == doe::config::ApplicationInputMode::kTaggedCopy) {
            if (!input.tagger) {
                throw RuntimeError("input", "", std::filesystem::path(),
                                   "tagged_copy input is missing its tagger");
            }
            plan.tagger = MakeTagger(*input.tagger);
        }
        inputs_.push_back(std::move(plan));
    }

    outputs_.reserve(config.outputs.size());
    for (auto& output : config.outputs) {
        Output plan;
        plan.port = std::move(output.port);
        plan.type = output.type;
        plan.path = std::move(output.path);
        plan.required = output.required;
        outputs_.push_back(std::move(plan));
    }
}

std::map<std::string, std::filesystem::path> Application::Run(
    const std::map<std::string, ScalarValue>& variable_values) const
{
    PrepareInputs(variable_values);
    Execute();
    std::map<std::string, std::filesystem::path> output_ports;
    CheckOutputs(output_ports);
    return output_ports;
}

std::string Application::ExpandTemplate(const std::string& text) const
{
    const std::map<std::string, std::filesystem::path> no_input_files;
    return ExpandPathTemplate(text, no_input_files);
}

std::filesystem::path Application::ExpandPath(const std::string& text) const
{
    return std::filesystem::path(ExpandTemplate(text));
}

void Application::PrepareInputs(const std::map<std::string, ScalarValue>& variable_values) const
{
    for (const auto& input : inputs_) {
        if (input.mode == doe::config::ApplicationInputMode::kCopy) {
            PrepareCopy(input);
        } else {
            PrepareTaggedCopy(input, variable_values);
        }
    }
}

void Application::PrepareCopy(const Input& input) const
{
    const std::filesystem::path source = ExpandPath(input.source);
    const std::filesystem::path destination = ExpandPath(input.destination);
    EnsureParentDirectories(destination);
    std::error_code error;
    std::filesystem::copy_file(source, destination,
                               std::filesystem::copy_options::overwrite_existing, error);
    if (error) {
        throw RuntimeError("input", "", destination,
                           "failed to copy input: " + error.message());
    }
}

void Application::PrepareTaggedCopy(const Input& input,
                                    const std::map<std::string, ScalarValue>& variable_values) const
{
    if (!input.tagger) {
        throw RuntimeError("input", "", std::filesystem::path(),
                           "tagged_copy input is missing its tagger");
    }
    const std::filesystem::path source = ExpandPath(input.source);
    const std::filesystem::path destination = ExpandPath(input.destination);
    const std::string template_text = ReadTextFile(source);
    const std::string output = input.tagger->Apply(template_text, variable_values);
    EnsureParentDirectories(destination);
    WriteTextFile(destination, output);
}

void Application::Execute() const
{
    const std::string executable = ExpandTemplate(executable_);
    const std::filesystem::path working_directory = ExpandPath(working_directory_);
    std::vector<std::string> arguments;
    arguments.reserve(arguments_.size());
    for (const std::string& argument : arguments_) {
        arguments.push_back(ExpandTemplate(argument));
    }

    Process process(std::filesystem::path(executable), working_directory, arguments,
                    timeout_seconds_);
    const ProcessResult result = process.Run();

    if (result.timed_out) {
        throw RuntimeError("process", executable, working_directory,
                           "process timed out after " + std::to_string(timeout_seconds_) +
                               " seconds");
    }
    const bool accepted =
        std::find(success_exit_codes_.begin(), success_exit_codes_.end(), result.exit_code) !=
        success_exit_codes_.end();
    if (!accepted) {
        throw RuntimeError("process", executable, working_directory,
                           "unexpected exit code " + std::to_string(result.exit_code));
    }
}

void Application::CheckOutputs(std::map<std::string, std::filesystem::path>& output_ports) const
{
    for (const auto& output : outputs_) {
        const std::filesystem::path path = ExpandPath(output.path);
        if (output.required && !std::filesystem::exists(path)) {
            throw RuntimeError("output", output.port, path,
                               "required output file does not exist");
        }
        output_ports.emplace(output.port, path);
    }
}

}  // namespace doe::runtime
