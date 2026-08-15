/********************************************************************
 * "extractor.h" declares the response Extractor runner.           *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#ifndef DOE_ENGINE_RUNTIME_EXTRACTOR_EXTRACTOR_H_
#define DOE_ENGINE_RUNTIME_EXTRACTOR_EXTRACTOR_H_

#include "extractor_config.h"
#include "scalar_value.h"

#include <filesystem>
#include <map>
#include <memory>
#include <string>

namespace doe::runtime {

// Consumes Application outputs through explicit port bindings and produces the
// observed response values (response id to scalar value) for one design point.
// A ExtractorRunner is constructed with the extractor instances and their
// definitions; it pre-compiles every instance and owns the runtime bindings, so
// Run() only needs the Application outputs. The run directories come from the
// global doe::utils::RunContext.
class ExtractorRunner {
public:
    virtual ~ExtractorRunner() = default;

    virtual std::map<std::string, ScalarValue> Run(
        const std::map<std::string, std::map<std::string, std::filesystem::path>>&
            application_outputs) const = 0;
};

// Factory for the first ExtractorRunner implementation.
std::unique_ptr<ExtractorRunner> MakeExtractorRunner(
    const doe::config::ExtractorsConfig& extractors,
    const std::map<std::string, doe::config::ExtractorDefinitionConfig>& definitions);

// One extractor type. Following the Tagger pattern, each serialized extractor
// type is a derived class, and MakeResponseExtractor turns an
// ExtractorDefinitionConfig into the matching implementation. Extract() takes
// the runtime output-port to response-id bindings of its instance, never the
// Config.
class ResponseExtractor {
public:
    virtual ~ResponseExtractor() = default;

    virtual std::map<std::string, ScalarValue> Extract(
        const std::string& instance_id,
        const std::map<std::string, std::string>& output_bindings,
        const std::map<std::string, std::filesystem::path>& input_files) const = 0;
};

// Factory for ResponseExtractor implementations. Concrete extractor types live
// in their own headers: delimited_text_extractor.h, python_script_extractor.h.
std::unique_ptr<ResponseExtractor> MakeResponseExtractor(
    const doe::config::ExtractorDefinitionConfig& definition);

}  // namespace doe::runtime

#endif  // DOE_ENGINE_RUNTIME_EXTRACTOR_EXTRACTOR_H_
