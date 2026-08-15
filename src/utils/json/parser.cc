/********************************************************************
 * "parser.cc" provides the JSON configuration Parser.             *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#include "parser.h"

#include "json.hpp"

#include "error.h"

#include <fstream>
#include <limits>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace doe::utils {

namespace {

using doe::config::ApplicationConfig;
using doe::config::ApplicationInstanceConfig;
using doe::config::ApplicationInputConfig;
using doe::config::ApplicationInputMode;
using doe::config::ApplicationOutputConfig;
using doe::config::ApplicationOutputReferenceConfig;
using doe::config::ApplicationType;
using doe::config::ArtifactType;
using doe::config::ConfigScalar;
using doe::config::ContinuousDomainConfig;
using doe::config::DelimitedTagConfig;
using doe::config::DelimitedTextTaggerConfig;
using doe::config::DiscreteDomainConfig;
using doe::config::DomainConfig;
using doe::config::DomainType;
using doe::config::ExperimentConfig;
using doe::config::ExtractorDefinitionConfig;
using doe::config::ExtractorInstanceConfig;
using doe::config::ExtractorInputPortConfig;
using doe::config::ExtractorOutputPortConfig;
using doe::config::ExtractorsConfig;
using doe::config::FactorConfig;
using doe::config::FileReference;
using doe::config::DelimitedExtractorTagConfig;
using doe::config::DelimitedTextExtractorConfig;
using doe::config::LoadedExperimentConfig;
using doe::config::PlaceholderTagConfig;
using doe::config::PlaceholderTextTaggerConfig;
using doe::config::PythonResultConfig;
using doe::config::PythonResultValueConfig;
using doe::config::PythonScriptExtractorConfig;
using doe::config::ResponseConfig;
using doe::config::ResponsesConfig;
using doe::config::ResponseReferenceConfig;
using doe::config::SamplingConfig;
using doe::config::SamplingType;
using doe::config::TaggerConfig;
using doe::config::ValueFormatConfig;
using doe::config::ValueNotation;
using doe::config::ValueType;
using doe::config::VariableConfig;
using doe::config::VariablesConfig;
using doe::config::WorkflowConfig;
using doe::config::WorkflowConnectionConfig;
using doe::config::WorkflowEndpointConfig;
using doe::log::ConfigError;

void RequireAbsolutePath(const std::filesystem::path& file)
{
    if (!file.is_absolute()) {
        throw ConfigError(file, "", "expected an absolute JSON file path");
    }
}

nlohmann::json LoadJson(const std::filesystem::path& file)
{
    RequireAbsolutePath(file);
    std::ifstream stream(file);
    if (!stream) {
        throw ConfigError(file, "", "failed to open the JSON file");
    }
    try {
        return nlohmann::json::parse(stream);
    } catch (const nlohmann::json::parse_error& error) {
        throw ConfigError(file, "", std::string("invalid JSON: ") + error.what());
    }
}

const nlohmann::json& RequireObject(const nlohmann::json& value,
                                    const std::filesystem::path& source,
                                    const std::string& location)
{
    if (!value.is_object()) {
        throw ConfigError(source, location, "expected a JSON object");
    }
    return value;
}

const nlohmann::json& RequireArray(const nlohmann::json& value,
                                   const std::filesystem::path& source,
                                   const std::string& location)
{
    if (!value.is_array()) {
        throw ConfigError(source, location, "expected a JSON array");
    }
    return value;
}

const nlohmann::json& RequireMember(const nlohmann::json& object,
                                    const std::string& key,
                                    const std::filesystem::path& source,
                                    const std::string& location)
{
    auto found = object.find(key);
    if (found == object.end()) {
        throw ConfigError(source, location, "missing required field \"" + key + "\"");
    }
    return *found;
}

std::string RequireString(const nlohmann::json& object,
                          const std::string& key,
                          const std::filesystem::path& source,
                          const std::string& location)
{
    const std::string field_location = location + "/" + key;
    const nlohmann::json& value = RequireMember(object, key, source, location);
    if (!value.is_string()) {
        throw ConfigError(source, field_location, "expected a string");
    }
    return value.get<std::string>();
}

long long RequireInteger(const nlohmann::json& object,
                         const std::string& key,
                         const std::filesystem::path& source,
                         const std::string& location)
{
    const std::string field_location = location + "/" + key;
    const nlohmann::json& value = RequireMember(object, key, source, location);
    if (!value.is_number_integer()) {
        throw ConfigError(source, field_location, "expected an integer");
    }
    return value.get<long long>();
}

int RequireInt(const nlohmann::json& object,
               const std::string& key,
               const std::filesystem::path& source,
               const std::string& location)
{
    const long long value = RequireInteger(object, key, source, location);
    if (value < static_cast<long long>(std::numeric_limits<int>::min()) ||
        value > static_cast<long long>(std::numeric_limits<int>::max())) {
        throw ConfigError(source, location + "/" + key, "integer value out of range");
    }
    return static_cast<int>(value);
}

std::size_t RequireSize(const nlohmann::json& object,
                        const std::string& key,
                        const std::filesystem::path& source,
                        const std::string& location)
{
    const long long value = RequireInteger(object, key, source, location);
    if (value < 0) {
        throw ConfigError(source, location + "/" + key, "expected a non-negative integer");
    }
    return static_cast<std::size_t>(value);
}

double RequireDouble(const nlohmann::json& object,
                     const std::string& key,
                     const std::filesystem::path& source,
                     const std::string& location)
{
    const std::string field_location = location + "/" + key;
    const nlohmann::json& value = RequireMember(object, key, source, location);
    if (!value.is_number()) {
        throw ConfigError(source, field_location, "expected a number");
    }
    return value.get<double>();
}

bool RequireBoolean(const nlohmann::json& object,
                    const std::string& key,
                    const std::filesystem::path& source,
                    const std::string& location)
{
    const std::string field_location = location + "/" + key;
    const nlohmann::json& value = RequireMember(object, key, source, location);
    if (!value.is_boolean()) {
        throw ConfigError(source, field_location, "expected a boolean");
    }
    return value.get<bool>();
}

std::vector<std::string> RequireStringArray(const nlohmann::json& object,
                                            const std::string& key,
                                            const std::filesystem::path& source,
                                            const std::string& location)
{
    const std::string array_location = location + "/" + key;
    const nlohmann::json& array =
        RequireArray(RequireMember(object, key, source, location), source, array_location);
    std::vector<std::string> result;
    result.reserve(array.size());
    for (std::size_t i = 0; i < array.size(); ++i) {
        const std::string item_location = array_location + "/" + std::to_string(i);
        if (!array[i].is_string()) {
            throw ConfigError(source, item_location, "expected a string");
        }
        result.push_back(array[i].get<std::string>());
    }
    return result;
}

std::vector<int> RequireIntArray(const nlohmann::json& object,
                                 const std::string& key,
                                 const std::filesystem::path& source,
                                 const std::string& location)
{
    const std::string array_location = location + "/" + key;
    const nlohmann::json& array =
        RequireArray(RequireMember(object, key, source, location), source, array_location);
    std::vector<int> result;
    result.reserve(array.size());
    for (std::size_t i = 0; i < array.size(); ++i) {
        const std::string item_location = array_location + "/" + std::to_string(i);
        if (!array[i].is_number_integer()) {
            throw ConfigError(source, item_location, "expected an integer");
        }
        const long long value = array[i].get<long long>();
        if (value < static_cast<long long>(std::numeric_limits<int>::min()) ||
            value > static_cast<long long>(std::numeric_limits<int>::max())) {
            throw ConfigError(source, item_location, "integer value out of range");
        }
        result.push_back(static_cast<int>(value));
    }
    return result;
}

ValueType ParseValueType(const std::string& value,
                         const std::filesystem::path& source,
                         const std::string& location)
{
    if (value == "real") {
        return ValueType::kReal;
    }
    if (value == "integer") {
        return ValueType::kInteger;
    }
    if (value == "string") {
        return ValueType::kString;
    }
    throw ConfigError(source, location, "unsupported value_type \"" + value + "\"");
}

DomainType ParseDomainType(const std::string& value,
                           const std::filesystem::path& source,
                           const std::string& location)
{
    if (value == "continuous") {
        return DomainType::kContinuous;
    }
    if (value == "discrete") {
        return DomainType::kDiscrete;
    }
    throw ConfigError(source, location, "unsupported domain type \"" + value + "\"");
}

SamplingType ParseSamplingType(const std::string& value,
                               const std::filesystem::path& source,
                               const std::string& location)
{
    if (value == "full_factorial") {
        return SamplingType::kFullFactorial;
    }
    throw ConfigError(source, location, "unsupported sampling type \"" + value + "\"");
}

ApplicationType ParseApplicationType(const std::string& value,
                                     const std::filesystem::path& source,
                                     const std::string& location)
{
    if (value == "command") {
        return ApplicationType::kCommand;
    }
    throw ConfigError(source, location, "unsupported application type \"" + value + "\"");
}

ApplicationInputMode ParseApplicationInputMode(const std::string& value,
                                               const std::filesystem::path& source,
                                               const std::string& location)
{
    if (value == "copy") {
        return ApplicationInputMode::kCopy;
    }
    if (value == "tagged_copy") {
        return ApplicationInputMode::kTaggedCopy;
    }
    throw ConfigError(source, location, "unsupported input mode \"" + value + "\"");
}

ArtifactType ParseArtifactType(const std::string& value,
                               const std::filesystem::path& source,
                               const std::string& location)
{
    if (value == "file") {
        return ArtifactType::kFile;
    }
    throw ConfigError(source, location, "unsupported artifact type \"" + value + "\"");
}

ValueNotation ParseValueNotation(const std::string& value,
                                 const std::filesystem::path& source,
                                 const std::string& location)
{
    if (value == "scientific") {
        return ValueNotation::kScientific;
    }
    if (value == "fixed") {
        return ValueNotation::kFixed;
    }
    if (value == "general") {
        return ValueNotation::kGeneral;
    }
    throw ConfigError(source, location, "unsupported value notation \"" + value + "\"");
}

FileReference ParseFileReference(const nlohmann::json& value,
                                 const std::filesystem::path& source,
                                 const std::string& location,
                                 const std::filesystem::path& base_directory)
{
    const nlohmann::json& object = RequireObject(value, source, location);
    std::string file = RequireString(object, "file", source, location);
    std::filesystem::path resolved(file);
    if (resolved.is_relative()) {
        resolved = base_directory / resolved;
    }
    FileReference reference;
    reference.file = resolved.lexically_normal();
    return reference;
}

ContinuousDomainConfig ParseContinuousDomain(const nlohmann::json& value,
                                             const std::filesystem::path& source,
                                             const std::string& location)
{
    const nlohmann::json& object = RequireObject(value, source, location);
    ContinuousDomainConfig domain;
    domain.type =
        ParseDomainType(RequireString(object, "type", source, location), source, location + "/type");
    domain.lower_bound = RequireDouble(object, "lower_bound", source, location);
    domain.upper_bound = RequireDouble(object, "upper_bound", source, location);
    return domain;
}

DiscreteDomainConfig ParseDiscreteDomain(const nlohmann::json& value,
                                         const std::filesystem::path& source,
                                         const std::string& location)
{
    const nlohmann::json& object = RequireObject(value, source, location);
    DiscreteDomainConfig domain;
    domain.type =
        ParseDomainType(RequireString(object, "type", source, location), source, location + "/type");
    const nlohmann::json& values = RequireArray(RequireMember(object, "values", source, location),
                                                source, location + "/values");
    domain.values.reserve(values.size());
    for (std::size_t i = 0; i < values.size(); ++i) {
        const std::string item_location = location + "/values/" + std::to_string(i);
        const nlohmann::json& item = values[i];
        ConfigScalar scalar;
        if (item.is_number_integer()) {
            const long long integer = item.get<long long>();
            if (integer < static_cast<long long>(std::numeric_limits<int>::min()) ||
                integer > static_cast<long long>(std::numeric_limits<int>::max())) {
                scalar = static_cast<double>(integer);
            } else {
                scalar = static_cast<int>(integer);
            }
        } else if (item.is_number()) {
            scalar = item.get<double>();
        } else if (item.is_string()) {
            scalar = item.get<std::string>();
        } else {
            throw ConfigError(source, item_location, "expected a number or a string");
        }
        domain.values.push_back(std::move(scalar));
    }
    return domain;
}

DomainConfig ParseDomain(const nlohmann::json& value, const std::filesystem::path& source,
                         const std::string& location)
{
    const nlohmann::json& object = RequireObject(value, source, location);
    const std::string type = RequireString(object, "type", source, location);
    if (type == "continuous") {
        return ParseContinuousDomain(value, source, location);
    }
    if (type == "discrete") {
        return ParseDiscreteDomain(value, source, location);
    }
    throw ConfigError(source, location + "/type", "unsupported domain type \"" + type + "\"");
}

std::vector<PlaceholderTagConfig> ParsePlaceholderTags(const nlohmann::json& value,
                                                       const std::filesystem::path& source,
                                                       const std::string& location)
{
    const nlohmann::json& array = RequireArray(value, source, location);
    std::vector<PlaceholderTagConfig> result;
    result.reserve(array.size());
    for (std::size_t i = 0; i < array.size(); ++i) {
        const std::string item_location = location + "/" + std::to_string(i);
        const nlohmann::json& item = RequireObject(array[i], source, item_location);
        PlaceholderTagConfig tag;
        tag.placeholder = RequireString(item, "placeholder", source, item_location);
        tag.variable = RequireString(item, "variable", source, item_location);
        result.push_back(std::move(tag));
    }
    return result;
}

std::vector<DelimitedTagConfig> ParseDelimitedTags(const nlohmann::json& value,
                                                   const std::filesystem::path& source,
                                                   const std::string& location)
{
    const nlohmann::json& array = RequireArray(value, source, location);
    std::vector<DelimitedTagConfig> result;
    result.reserve(array.size());
    for (std::size_t i = 0; i < array.size(); ++i) {
        const std::string item_location = location + "/" + std::to_string(i);
        const nlohmann::json& item = RequireObject(array[i], source, item_location);
        DelimitedTagConfig tag;
        tag.row = RequireSize(item, "row", source, item_location);
        tag.column = RequireSize(item, "column", source, item_location);
        tag.variable = RequireString(item, "variable", source, item_location);
        result.push_back(std::move(tag));
    }
    return result;
}

std::vector<DelimitedExtractorTagConfig> ParseDelimitedExtractorTags(
    const nlohmann::json& value, const std::filesystem::path& source, const std::string& location)
{
    const nlohmann::json& array = RequireArray(value, source, location);
    std::vector<DelimitedExtractorTagConfig> result;
    result.reserve(array.size());
    for (std::size_t i = 0; i < array.size(); ++i) {
        const std::string item_location = location + "/" + std::to_string(i);
        const nlohmann::json& item = RequireObject(array[i], source, item_location);
        DelimitedExtractorTagConfig tag;
        tag.row = RequireSize(item, "row", source, item_location);
        tag.column = RequireSize(item, "column", source, item_location);
        tag.output = RequireString(item, "output", source, item_location);
        result.push_back(std::move(tag));
    }
    return result;
}

ValueFormatConfig ParseValueFormat(const nlohmann::json& value,
                                   const std::filesystem::path& source,
                                   const std::string& location)
{
    const nlohmann::json& object = RequireObject(value, source, location);
    ValueFormatConfig format;
    format.notation = ParseValueNotation(RequireString(object, "notation", source, location),
                                         source, location + "/notation");
    format.precision = RequireInt(object, "precision", source, location);
    return format;
}

TaggerConfig ParseTaggerConfig(const nlohmann::json& value,
                               const std::filesystem::path& source,
                               const std::string& location)
{
    const nlohmann::json& object = RequireObject(value, source, location);
    const std::string type = RequireString(object, "type", source, location);
    const std::string type_location = location + "/type";
    if (type == "placeholder_text") {
        PlaceholderTextTaggerConfig tagger;
        tagger.tags = ParsePlaceholderTags(RequireMember(object, "tags", source, location),
                                           source, location + "/tags");
        return tagger;
    }
    if (type == "delimited_text") {
        DelimitedTextTaggerConfig tagger;
        tagger.index_base = RequireSize(object, "index_base", source, location);
        tagger.delimiters = RequireStringArray(object, "delimiters", source, location);
        tagger.value_format =
            ParseValueFormat(RequireMember(object, "value_format", source, location),
                             source, location + "/value_format");
        tagger.tags = ParseDelimitedTags(RequireMember(object, "tags", source, location),
                                         source, location + "/tags");
        return tagger;
    }
    throw ConfigError(source, type_location, "unsupported tagger type \"" + type + "\"");
}

std::vector<ApplicationInputConfig> ParseApplicationInputs(const nlohmann::json& value,
                                                           const std::filesystem::path& source,
                                                           const std::string& location)
{
    const nlohmann::json& array = RequireArray(value, source, location);
    std::vector<ApplicationInputConfig> result;
    result.reserve(array.size());
    for (std::size_t i = 0; i < array.size(); ++i) {
        const std::string item_location = location + "/" + std::to_string(i);
        const nlohmann::json& item = RequireObject(array[i], source, item_location);
        ApplicationInputConfig input;
        input.mode = ParseApplicationInputMode(RequireString(item, "mode", source, item_location),
                                               source, item_location + "/mode");
        input.source = RequireString(item, "source", source, item_location);
        input.destination = RequireString(item, "destination", source, item_location);
        auto tagger = item.find("tagger");
        if (input.mode == ApplicationInputMode::kTaggedCopy) {
            if (tagger == item.end()) {
                throw ConfigError(source, item_location,
                                  "tagged_copy input requires a \"tagger\"");
            }
            input.tagger = ParseTaggerConfig(*tagger, source, item_location + "/tagger");
        } else {
            if (tagger != item.end()) {
                throw ConfigError(source, item_location,
                                  "copy input must not declare a \"tagger\"");
            }
            input.tagger = std::nullopt;
        }
        result.push_back(std::move(input));
    }
    return result;
}

std::vector<ApplicationOutputConfig> ParseApplicationOutputs(const nlohmann::json& value,
                                                             const std::filesystem::path& source,
                                                             const std::string& location)
{
    const nlohmann::json& array = RequireArray(value, source, location);
    std::vector<ApplicationOutputConfig> result;
    result.reserve(array.size());
    for (std::size_t i = 0; i < array.size(); ++i) {
        const std::string item_location = location + "/" + std::to_string(i);
        const nlohmann::json& item = RequireObject(array[i], source, item_location);
        ApplicationOutputConfig output;
        output.port = RequireString(item, "port", source, item_location);
        output.type = ParseArtifactType(RequireString(item, "type", source, item_location),
                                        source, item_location + "/type");
        output.path = RequireString(item, "path", source, item_location);
        output.required = RequireBoolean(item, "required", source, item_location);
        result.push_back(std::move(output));
    }
    return result;
}

std::vector<ExtractorInputPortConfig> ParseExtractorInputPorts(const nlohmann::json& value,
                                                               const std::filesystem::path& source,
                                                               const std::string& location)
{
    const nlohmann::json& array = RequireArray(value, source, location);
    std::vector<ExtractorInputPortConfig> result;
    result.reserve(array.size());
    for (std::size_t i = 0; i < array.size(); ++i) {
        const std::string item_location = location + "/" + std::to_string(i);
        const nlohmann::json& item = RequireObject(array[i], source, item_location);
        ExtractorInputPortConfig port;
        port.port = RequireString(item, "port", source, item_location);
        port.type = ParseArtifactType(RequireString(item, "type", source, item_location),
                                      source, item_location + "/type");
        result.push_back(std::move(port));
    }
    return result;
}

std::vector<ExtractorOutputPortConfig> ParseExtractorOutputPorts(const nlohmann::json& value,
                                                                 const std::filesystem::path& source,
                                                                 const std::string& location)
{
    const nlohmann::json& array = RequireArray(value, source, location);
    std::vector<ExtractorOutputPortConfig> result;
    result.reserve(array.size());
    for (std::size_t i = 0; i < array.size(); ++i) {
        const std::string item_location = location + "/" + std::to_string(i);
        const nlohmann::json& item = RequireObject(array[i], source, item_location);
        ExtractorOutputPortConfig port;
        port.port = RequireString(item, "port", source, item_location);
        port.value_type = ParseValueType(RequireString(item, "value_type", source, item_location),
                                         source, item_location + "/value_type");
        result.push_back(std::move(port));
    }
    return result;
}

std::vector<PythonResultValueConfig> ParsePythonResultValues(const nlohmann::json& value,
                                                             const std::filesystem::path& source,
                                                             const std::string& location)
{
    const nlohmann::json& array = RequireArray(value, source, location);
    std::vector<PythonResultValueConfig> result;
    result.reserve(array.size());
    for (std::size_t i = 0; i < array.size(); ++i) {
        const std::string item_location = location + "/" + std::to_string(i);
        const nlohmann::json& item = RequireObject(array[i], source, item_location);
        PythonResultValueConfig entry;
        entry.key = RequireString(item, "key", source, item_location);
        entry.output = RequireString(item, "output", source, item_location);
        entry.value_type = ParseValueType(RequireString(item, "value_type", source, item_location),
                                          source, item_location + "/value_type");
        result.push_back(std::move(entry));
    }
    return result;
}

PythonResultConfig ParsePythonResult(const nlohmann::json& value,
                                     const std::filesystem::path& source,
                                     const std::string& location)
{
    const nlohmann::json& object = RequireObject(value, source, location);
    PythonResultConfig result;
    result.file = RequireString(object, "file", source, location);
    result.format = RequireString(object, "format", source, location);
    result.values = ParsePythonResultValues(RequireMember(object, "values", source, location),
                                            source, location + "/values");
    return result;
}

std::map<std::string, ApplicationOutputReferenceConfig> ParseApplicationOutputReferenceMap(
    const nlohmann::json& value,
    const std::filesystem::path& source,
    const std::string& location)
{
    const nlohmann::json& object = RequireObject(value, source, location);
    std::map<std::string, ApplicationOutputReferenceConfig> result;
    for (auto entry = object.begin(); entry != object.end(); ++entry) {
        const std::string entry_location = location + "/" + entry.key();
        const nlohmann::json& item = RequireObject(entry.value(), source, entry_location);
        ApplicationOutputReferenceConfig reference;
        reference.application = RequireString(item, "application", source, entry_location);
        reference.output = RequireString(item, "output", source, entry_location);
        result.emplace(entry.key(), std::move(reference));
    }
    return result;
}

std::map<std::string, ResponseReferenceConfig> ParseResponseReferenceMap(
    const nlohmann::json& value,
    const std::filesystem::path& source,
    const std::string& location)
{
    const nlohmann::json& object = RequireObject(value, source, location);
    std::map<std::string, ResponseReferenceConfig> result;
    for (auto entry = object.begin(); entry != object.end(); ++entry) {
        const std::string entry_location = location + "/" + entry.key();
        const nlohmann::json& item = RequireObject(entry.value(), source, entry_location);
        ResponseReferenceConfig reference;
        reference.response = RequireString(item, "response", source, entry_location);
        result.emplace(entry.key(), std::move(reference));
    }
    return result;
}

WorkflowEndpointConfig ParseWorkflowEndpoint(const nlohmann::json& value,
                                             const std::filesystem::path& source,
                                             const std::string& location)
{
    const nlohmann::json& object = RequireObject(value, source, location);
    WorkflowEndpointConfig endpoint;
    endpoint.application = RequireString(object, "application", source, location);
    endpoint.port = RequireString(object, "port", source, location);
    return endpoint;
}

}  // namespace

ExperimentConfig Parser::ParseExperiment(const std::filesystem::path& file)
{
    nlohmann::json root = LoadJson(file);
    const std::filesystem::path source = file;
    RequireObject(root, source, "");
    const std::filesystem::path base_directory = file.parent_path();

    ExperimentConfig config;
    config.format = RequireString(root, "format", source, "");
    config.format_version = RequireInt(root, "format_version", source, "");
    config.id = RequireString(root, "id", source, "");
    config.name = RequireString(root, "name", source, "");
    config.description = RequireString(root, "description", source, "");
    config.variables = ParseFileReference(RequireMember(root, "variables", source, ""),
                                          source, "/variables", base_directory);
    config.responses = ParseFileReference(RequireMember(root, "responses", source, ""),
                                          source, "/responses", base_directory);
    config.sampling = ParseFileReference(RequireMember(root, "sampling", source, ""),
                                         source, "/sampling", base_directory);
    config.workflow = ParseFileReference(RequireMember(root, "workflow", source, ""),
                                         source, "/workflow", base_directory);
    config.extractors = ParseFileReference(RequireMember(root, "extractors", source, ""),
                                           source, "/extractors", base_directory);
    return config;
}

VariablesConfig Parser::ParseVariables(const std::filesystem::path& file)
{
    nlohmann::json root = LoadJson(file);
    const std::filesystem::path source = file;
    RequireObject(root, source, "");
    const nlohmann::json& variables = RequireArray(RequireMember(root, "variables", source, ""),
                                                   source, "/variables");

    VariablesConfig config;
    config.variables.reserve(variables.size());
    for (std::size_t i = 0; i < variables.size(); ++i) {
        const std::string location = "/variables/" + std::to_string(i);
        const nlohmann::json& item = RequireObject(variables[i], source, location);
        VariableConfig variable;
        variable.id = RequireString(item, "id", source, location);
        variable.name = RequireString(item, "name", source, location);
        variable.description = RequireString(item, "description", source, location);
        variable.unit = RequireString(item, "unit", source, location);
        variable.value_type =
            ParseValueType(RequireString(item, "value_type", source, location),
                           source, location + "/value_type");
        variable.domain =
            ParseDomain(RequireMember(item, "domain", source, location),
                        source, location + "/domain");
        config.variables.push_back(std::move(variable));
    }
    return config;
}

ResponsesConfig Parser::ParseResponses(const std::filesystem::path& file)
{
    nlohmann::json root = LoadJson(file);
    const std::filesystem::path source = file;
    RequireObject(root, source, "");
    const nlohmann::json& responses = RequireArray(RequireMember(root, "responses", source, ""),
                                                   source, "/responses");

    ResponsesConfig config;
    config.responses.reserve(responses.size());
    for (std::size_t i = 0; i < responses.size(); ++i) {
        const std::string location = "/responses/" + std::to_string(i);
        const nlohmann::json& item = RequireObject(responses[i], source, location);
        ResponseConfig response;
        response.id = RequireString(item, "id", source, location);
        response.name = RequireString(item, "name", source, location);
        response.description = RequireString(item, "description", source, location);
        response.unit = RequireString(item, "unit", source, location);
        response.value_type =
            ParseValueType(RequireString(item, "value_type", source, location),
                           source, location + "/value_type");
        config.responses.push_back(std::move(response));
    }
    return config;
}

SamplingConfig Parser::ParseSampling(const std::filesystem::path& file)
{
    nlohmann::json root = LoadJson(file);
    const std::filesystem::path source = file;
    RequireObject(root, source, "");

    SamplingConfig config;
    config.type = ParseSamplingType(RequireString(root, "type", source, ""), source, "/type");
    const nlohmann::json& factors = RequireArray(RequireMember(root, "factors", source, ""),
                                                 source, "/factors");
    config.factors.reserve(factors.size());
    for (std::size_t i = 0; i < factors.size(); ++i) {
        const std::string location = "/factors/" + std::to_string(i);
        const nlohmann::json& item = RequireObject(factors[i], source, location);
        FactorConfig factor;
        factor.variable = RequireString(item, "variable", source, location);
        factor.levels = RequireSize(item, "levels", source, location);
        config.factors.push_back(std::move(factor));
    }
    return config;
}

WorkflowConfig Parser::ParseWorkflow(const std::filesystem::path& file)
{
    nlohmann::json root = LoadJson(file);
    const std::filesystem::path source = file;
    RequireObject(root, source, "");
    const std::filesystem::path base_directory = file.parent_path();

    WorkflowConfig config;
    const nlohmann::json& applications = RequireArray(RequireMember(root, "applications", source, ""),
                                                      source, "/applications");
    config.applications.reserve(applications.size());
    for (std::size_t i = 0; i < applications.size(); ++i) {
        const std::string location = "/applications/" + std::to_string(i);
        const nlohmann::json& item = RequireObject(applications[i], source, location);
        ApplicationInstanceConfig instance;
        instance.id = RequireString(item, "id", source, location);
        instance.configuration =
            ParseFileReference(RequireMember(item, "configuration", source, location),
                               source, location + "/configuration", base_directory);
        config.applications.push_back(std::move(instance));
    }

    const nlohmann::json& connections = RequireArray(RequireMember(root, "connections", source, ""),
                                                     source, "/connections");
    config.connections.reserve(connections.size());
    for (std::size_t i = 0; i < connections.size(); ++i) {
        const std::string location = "/connections/" + std::to_string(i);
        const nlohmann::json& item = RequireObject(connections[i], source, location);
        WorkflowConnectionConfig connection;
        connection.from =
            ParseWorkflowEndpoint(RequireMember(item, "from", source, location),
                                  source, location + "/from");
        connection.to = ParseWorkflowEndpoint(RequireMember(item, "to", source, location),
                                              source, location + "/to");
        config.connections.push_back(std::move(connection));
    }
    return config;
}

ExtractorsConfig Parser::ParseExtractors(const std::filesystem::path& file)
{
    nlohmann::json root = LoadJson(file);
    const std::filesystem::path source = file;
    RequireObject(root, source, "");
    const std::filesystem::path base_directory = file.parent_path();

    ExtractorsConfig config;
    const nlohmann::json& extractors = RequireArray(RequireMember(root, "extractors", source, ""),
                                                    source, "/extractors");
    config.extractors.reserve(extractors.size());
    for (std::size_t i = 0; i < extractors.size(); ++i) {
        const std::string location = "/extractors/" + std::to_string(i);
        const nlohmann::json& item = RequireObject(extractors[i], source, location);
        ExtractorInstanceConfig instance;
        instance.id = RequireString(item, "id", source, location);
        instance.configuration =
            ParseFileReference(RequireMember(item, "configuration", source, location),
                               source, location + "/configuration", base_directory);
        instance.inputs = ParseApplicationOutputReferenceMap(
            RequireMember(item, "inputs", source, location), source, location + "/inputs");
        instance.outputs = ParseResponseReferenceMap(
            RequireMember(item, "outputs", source, location), source, location + "/outputs");
        config.extractors.push_back(std::move(instance));
    }
    return config;
}

ApplicationConfig Parser::ParseApplication(const std::filesystem::path& file)
{
    nlohmann::json root = LoadJson(file);
    const std::filesystem::path source = file;
    RequireObject(root, source, "");

    ApplicationConfig config;
    config.type = ParseApplicationType(RequireString(root, "type", source, ""), source, "/type");
    config.executable = RequireString(root, "executable", source, "");
    config.working_directory = RequireString(root, "working_directory", source, "");
    config.arguments = RequireStringArray(root, "arguments", source, "");
    config.timeout_seconds = RequireInteger(root, "timeout_seconds", source, "");
    config.success_exit_codes = RequireIntArray(root, "success_exit_codes", source, "");
    config.inputs = ParseApplicationInputs(RequireMember(root, "inputs", source, ""),
                                           source, "/inputs");
    config.outputs = ParseApplicationOutputs(RequireMember(root, "outputs", source, ""),
                                             source, "/outputs");
    return config;
}

ExtractorDefinitionConfig Parser::ParseExtractorDefinition(const std::filesystem::path& file)
{
    nlohmann::json root = LoadJson(file);
    const std::filesystem::path source = file;
    RequireObject(root, source, "");

    const std::string type = RequireString(root, "type", source, "");
    const std::string type_location = "/type";
    if (type == "delimited_text") {
        DelimitedTextExtractorConfig config;
        config.inputs = ParseExtractorInputPorts(RequireMember(root, "inputs", source, ""),
                                                 source, "/inputs");
        config.index_base = RequireSize(root, "index_base", source, "");
        config.delimiters = RequireStringArray(root, "delimiters", source, "");
        config.tags = ParseDelimitedExtractorTags(RequireMember(root, "tags", source, ""),
                                                  source, "/tags");
        config.outputs = ParseExtractorOutputPorts(RequireMember(root, "outputs", source, ""),
                                                   source, "/outputs");
        return config;
    }
    if (type == "python_script") {
        PythonScriptExtractorConfig config;
        config.interpreter = RequireString(root, "interpreter", source, "");
        config.script = RequireString(root, "script", source, "");
        config.working_directory = RequireString(root, "working_directory", source, "");
        config.inputs = ParseExtractorInputPorts(RequireMember(root, "inputs", source, ""),
                                                 source, "/inputs");
        config.arguments = RequireStringArray(root, "arguments", source, "");
        config.timeout_seconds = RequireInteger(root, "timeout_seconds", source, "");
        config.success_exit_codes = RequireIntArray(root, "success_exit_codes", source, "");
        config.result = ParsePythonResult(RequireMember(root, "result", source, ""),
                                          source, "/result");
        return config;
    }
    throw ConfigError(source, type_location, "unsupported extractor type \"" + type + "\"");
}

LoadedExperimentConfig Parser::LoadExperiment(const std::filesystem::path& experiment_file)
{
    RequireAbsolutePath(experiment_file);

    LoadedExperimentConfig loaded;
    loaded.experiment_file = experiment_file;
    loaded.experiment = ParseExperiment(experiment_file);
    loaded.variables = ParseVariables(loaded.experiment.variables.file);
    loaded.responses = ParseResponses(loaded.experiment.responses.file);
    loaded.sampling = ParseSampling(loaded.experiment.sampling.file);
    loaded.workflow = ParseWorkflow(loaded.experiment.workflow.file);
    loaded.extractors = ParseExtractors(loaded.experiment.extractors.file);

    for (const auto& instance : loaded.workflow.applications) {
        auto inserted = loaded.applications.emplace(
            instance.id, ParseApplication(instance.configuration.file));
        if (!inserted.second) {
            throw ConfigError(experiment_file, "",
                              "duplicate Application instance id \"" + instance.id + "\"");
        }
    }
    for (const auto& instance : loaded.extractors.extractors) {
        auto inserted = loaded.extractor_definitions.emplace(
            instance.id, ParseExtractorDefinition(instance.configuration.file));
        if (!inserted.second) {
            throw ConfigError(experiment_file, "",
                              "duplicate Extractor instance id \"" + instance.id + "\"");
        }
    }
    return loaded;
}

}  // namespace doe::utils
