/********************************************************************
 * "application_config.h" defines Application and Tagger configs.   *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#ifndef DOE_ENGINE_CONFIG_APPLICATION_CONFIG_H_
#define DOE_ENGINE_CONFIG_APPLICATION_CONFIG_H_

#include "config_types.h"

#include <cstddef>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace doe::config {

struct PlaceholderTagConfig {
    std::string placeholder;
    std::string variable;
};

struct PlaceholderTextTaggerConfig {
    std::vector<PlaceholderTagConfig> tags;
};

struct ValueFormatConfig {
    ValueNotation notation = ValueNotation::kGeneral;
    int precision = 0;
};

struct DelimitedTagConfig {
    std::size_t row = 0;
    std::size_t column = 0;
    std::string variable;
};

struct DelimitedTextTaggerConfig {
    // The same base is applied to both row and column indexes.
    std::size_t index_base = 0;
    std::vector<std::string> delimiters;
    ValueFormatConfig value_format;
    std::vector<DelimitedTagConfig> tags;
};

using TaggerConfig =
    std::variant<PlaceholderTextTaggerConfig, DelimitedTextTaggerConfig>;

struct ApplicationInputConfig {
    ApplicationInputMode mode = ApplicationInputMode::kCopy;

    // Runtime path templates are strings rather than filesystem::path because
    // placeholders such as {experiment_dir} have not been expanded yet.
    std::string source;
    std::string destination;

    // Required for tagged_copy and absent for a plain copy.
    std::optional<TaggerConfig> tagger;
};

struct ApplicationOutputConfig {
    // The logical port is used by Workflow and Extractor bindings. The path is
    // the physical file produced for one Design Point.
    std::string port;
    ArtifactType type = ArtifactType::kFile;
    std::string path;
    bool required = true;
};

// Serialized by a file under applications/. The first implementation supports
// command-line Applications only.
struct ApplicationConfig {
    ApplicationType type = ApplicationType::kCommand;
    std::string executable;
    std::string working_directory;
    std::vector<std::string> arguments;
    long long timeout_seconds = 0;
    std::vector<int> success_exit_codes;
    std::vector<ApplicationInputConfig> inputs;
    std::vector<ApplicationOutputConfig> outputs;
};

}  // namespace doe::config

#endif  // DOE_ENGINE_CONFIG_APPLICATION_CONFIG_H_
