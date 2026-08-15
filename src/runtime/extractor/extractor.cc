/********************************************************************
 * "extractor.cc" provides the response Extractor runner.          *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#include "extractor.h"

#include "delimited_text_extractor.h"
#include "error.h"
#include "python_script_extractor.h"

#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace doe::runtime {

using doe::log::RuntimeError;

std::unique_ptr<ResponseExtractor> MakeResponseExtractor(
    const doe::config::ExtractorDefinitionConfig& definition)
{
    if (std::holds_alternative<doe::config::DelimitedTextExtractorConfig>(definition)) {
        const doe::config::DelimitedTextExtractorConfig& source =
            std::get<doe::config::DelimitedTextExtractorConfig>(definition);
        std::vector<std::string> input_ports;
        input_ports.reserve(source.inputs.size());
        for (const auto& port : source.inputs) {
            input_ports.push_back(port.port);
        }
        std::vector<DelimitedExtractorTag> tags;
        tags.reserve(source.tags.size());
        for (const auto& tag : source.tags) {
            tags.push_back({tag.row, tag.column, tag.output});
        }
        std::vector<ExtractorOutput> outputs;
        outputs.reserve(source.outputs.size());
        for (const auto& output : source.outputs) {
            outputs.push_back({output.port, output.value_type});
        }
        return std::make_unique<DelimitedTextExtractor>(
            std::move(input_ports), source.index_base, source.delimiters, std::move(tags),
            std::move(outputs));
    }
    const doe::config::PythonScriptExtractorConfig& source =
        std::get<doe::config::PythonScriptExtractorConfig>(definition);
    if (source.result.format != "json") {
        throw RuntimeError("extractor", "", std::filesystem::path(),
                           "unsupported result format \"" + source.result.format + "\"");
    }
    std::vector<PythonResultValue> values;
    values.reserve(source.result.values.size());
    for (const auto& value : source.result.values) {
        values.push_back({value.key, value.output, value.value_type});
    }
    return std::make_unique<PythonScriptExtractor>(
        source.interpreter, source.script, source.working_directory, source.arguments,
        source.timeout_seconds, source.success_exit_codes, source.result.file, std::move(values));
}

namespace {

// Runtime form of one extractor instance: its id, the input port bindings, the
// output-port to response-id bindings, and the compiled extractor.
struct InputBinding {
    std::string application;
    std::string output;
};

struct Instance {
    std::string id;
    std::map<std::string, InputBinding> inputs;
    std::map<std::string, std::string> outputs;
    std::unique_ptr<ResponseExtractor> extractor;
};

class ExtractorRunnerImpl : public ExtractorRunner {
public:
    ExtractorRunnerImpl(const doe::config::ExtractorsConfig& extractors,
                        const std::map<std::string, doe::config::ExtractorDefinitionConfig>&
                            definitions)
    {
        instances_.reserve(extractors.extractors.size());
        for (const auto& instance : extractors.extractors) {
            const auto definition_it = definitions.find(instance.id);
            if (definition_it == definitions.end()) {
                throw RuntimeError("extractor", instance.id, std::filesystem::path(),
                                   "missing extractor definition");
            }
            Instance plan;
            plan.id = instance.id;
            for (const auto& binding : instance.inputs) {
                InputBinding input;
                input.application = binding.second.application;
                input.output = binding.second.output;
                plan.inputs.emplace(binding.first, std::move(input));
            }
            for (const auto& binding : instance.outputs) {
                plan.outputs.emplace(binding.first, binding.second.response);
            }
            plan.extractor = MakeResponseExtractor(definition_it->second);
            instances_.push_back(std::move(plan));
        }
    }

    std::map<std::string, ScalarValue> Run(
        const std::map<std::string, std::map<std::string, std::filesystem::path>>&
            application_outputs) const override
    {
        std::map<std::string, ScalarValue> responses;
        for (const auto& instance : instances_) {
            std::map<std::string, std::filesystem::path> input_files;
            for (const auto& binding : instance.inputs) {
                const auto app = application_outputs.find(binding.second.application);
                if (app == application_outputs.end()) {
                    throw RuntimeError("extractor", instance.id, std::filesystem::path(),
                                       "application \"" + binding.second.application +
                                           "\" produced no outputs");
                }
                const auto output = app->second.find(binding.second.output);
                if (output == app->second.end()) {
                    throw RuntimeError("extractor", instance.id, std::filesystem::path(),
                                       "application output port \"" + binding.second.output +
                                           "\" was not recorded");
                }
                input_files.emplace(binding.first, output->second);
            }

            const std::map<std::string, ScalarValue> values =
                instance.extractor->Extract(instance.id, instance.outputs, input_files);
            responses.insert(values.begin(), values.end());
        }
        return responses;
    }

private:
    std::vector<Instance> instances_;
};

}  // namespace

std::unique_ptr<ExtractorRunner> MakeExtractorRunner(
    const doe::config::ExtractorsConfig& extractors,
    const std::map<std::string, doe::config::ExtractorDefinitionConfig>& definitions)
{
    return std::make_unique<ExtractorRunnerImpl>(extractors, definitions);
}

}  // namespace doe::runtime
