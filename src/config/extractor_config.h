/********************************************************************
 * "extractor_config.h" defines Extractor configuration data.       *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#ifndef DOE_ENGINE_CONFIG_EXTRACTOR_CONFIG_H_
#define DOE_ENGINE_CONFIG_EXTRACTOR_CONFIG_H_

#include "config_types.h"

#include <cstddef>
#include <map>
#include <string>
#include <variant>
#include <vector>

namespace doe::config {

struct ApplicationOutputReferenceConfig {
    std::string application;
    std::string output;
};

struct ResponseReferenceConfig {
    std::string response;
};

// One entry in extractors.json. The keys in inputs and outputs are logical port
// names declared by the referenced Extractor definition.
struct ExtractorInstanceConfig {
    std::string id;
    FileReference configuration;
    std::map<std::string, ApplicationOutputReferenceConfig> inputs;
    std::map<std::string, ResponseReferenceConfig> outputs;
};

// Serialized by extractors.json.
struct ExtractorsConfig {
    std::vector<ExtractorInstanceConfig> extractors;
};

struct ExtractorInputPortConfig {
    std::string port;
    ArtifactType type = ArtifactType::kFile;
};

struct ExtractorOutputPortConfig {
    std::string port;
    ValueType value_type = ValueType::kReal;
};

// One entry in a delimited_text extractor: the cell at (row, column) of the
// source text feeds the logical output port it names.
struct DelimitedExtractorTagConfig {
    std::size_t row = 0;
    std::size_t column = 0;
    std::string output;
};

// Serialized by a delimited_text Extractor definition. It is the reverse of a
// delimited_text Tagger: instead of writing variable values into a template, it
// splits an output text file on delimiters and maps the cell at each (row,
// column) to an output port.
struct DelimitedTextExtractorConfig {
    std::vector<ExtractorInputPortConfig> inputs;

    // The same base is applied to both row and column indexes.
    std::size_t index_base = 0;
    std::vector<std::string> delimiters;
    std::vector<DelimitedExtractorTagConfig> tags;
    std::vector<ExtractorOutputPortConfig> outputs;
};

struct PythonResultValueConfig {
    std::string key;
    std::string output;
    ValueType value_type = ValueType::kReal;
};

struct PythonResultConfig {
    std::string file;
    std::string format;
    std::vector<PythonResultValueConfig> values;
};

// Serialized by a python_script Extractor definition. The declared logical
// output ports are the output fields in result.values.
struct PythonScriptExtractorConfig {
    std::string interpreter;
    std::string script;
    std::string working_directory;
    std::vector<ExtractorInputPortConfig> inputs;
    std::vector<std::string> arguments;
    long long timeout_seconds = 0;
    std::vector<int> success_exit_codes;
    PythonResultConfig result;
};

using ExtractorDefinitionConfig =
    std::variant<DelimitedTextExtractorConfig, PythonScriptExtractorConfig>;

}  // namespace doe::config

#endif  // DOE_ENGINE_CONFIG_EXTRACTOR_CONFIG_H_
