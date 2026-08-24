/********************************************************************
 * "parser.h" declares the JSON configuration Parser.              *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#ifndef DOE_ENGINE_UTILS_JSON_PARSER_H_
#define DOE_ENGINE_UTILS_JSON_PARSER_H_

#include "config.h"

#include <filesystem>

namespace doe::utils {

// Loads a single JSON configuration file at an absolute path and deserializes
// it into the matching declarative Config type. A referenced JSON file is
// resolved relative to the file containing the reference. Parser is stateless;
// failures are reported as doe::log::ConfigError with the source file and the
// offending JSON field location.
class Parser {
public:
    static doe::config::EngineConfig ParseEngineConfig(const std::filesystem::path& file);
    static doe::config::ExperimentConfig ParseExperiment(const std::filesystem::path& file);
    static doe::config::VariablesConfig ParseVariables(const std::filesystem::path& file);
    static doe::config::ResponsesConfig ParseResponses(const std::filesystem::path& file);
    static doe::config::SamplingConfig ParseSampling(const std::filesystem::path& file);
    static doe::config::WorkflowConfig ParseWorkflow(const std::filesystem::path& file);
    static doe::config::ExtractorsConfig ParseExtractors(const std::filesystem::path& file);
    static doe::config::ApplicationConfig ParseApplication(const std::filesystem::path& file);
    static doe::config::ExtractorDefinitionConfig ParseExtractorDefinition(
        const std::filesystem::path& file);

    // Loads experiment_file and every referenced configuration file,
    // producing the complete LoadedExperimentConfig.
    static doe::config::LoadedExperimentConfig LoadExperiment(
        const std::filesystem::path& experiment_file);
};

}  // namespace doe::utils

#endif  // DOE_ENGINE_UTILS_JSON_PARSER_H_
