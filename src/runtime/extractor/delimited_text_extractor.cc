/********************************************************************
 * "delimited_text_extractor.cc" provides the delimited_text type. *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#include "delimited_text_extractor.h"

#include "delimited_text.h"
#include "error.h"
#include "extractor_util.h"

#include <filesystem>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace doe::runtime {

using doe::utils::FindDelimitedToken;
using doe::utils::SplitTextLines;

DelimitedTextExtractor::DelimitedTextExtractor(std::vector<std::string> input_ports,
                                               std::size_t index_base,
                                               std::vector<std::string> delimiters,
                                               std::vector<DelimitedExtractorTag> tags,
                                               std::vector<ExtractorOutput> outputs)
    : input_ports_(std::move(input_ports)),
      index_base_(index_base),
      delimiters_(std::move(delimiters)),
      tags_(std::move(tags)),
      outputs_(std::move(outputs))
{
}

std::map<std::string, ScalarValue> DelimitedTextExtractor::Extract(
    const std::string& instance_id,
    const std::map<std::string, std::string>& output_bindings,
    const std::map<std::string, std::filesystem::path>& input_files) const
{
    if (input_ports_.empty()) {
        throw doe::log::RuntimeError("extractor", instance_id, std::filesystem::path(),
                                     "delimited_text extractor declares no input port");
    }
    const auto source_it = input_files.find(input_ports_.front());
    if (source_it == input_files.end()) {
        throw doe::log::RuntimeError("extractor", instance_id, std::filesystem::path(),
                                     "input port \"" + input_ports_.front() +
                                         "\" has no bound file");
    }
    const std::filesystem::path& source = source_it->second;
    const std::vector<std::string> lines = SplitTextLines(ReadTextFile(source));

    std::map<std::string, ScalarValue> responses;
    for (const auto& tag : tags_) {
        if (tag.row < index_base_) {
            throw doe::log::RuntimeError("extractor", instance_id, source,
                                         "row index below index_base");
        }
        const std::size_t row = tag.row - index_base_;
        if (row >= lines.size()) {
            throw doe::log::RuntimeError("extractor", instance_id, source,
                                         "row index out of range");
        }
        if (tag.column < index_base_) {
            throw doe::log::RuntimeError("extractor", instance_id, source,
                                         "column index below index_base");
        }
        const std::size_t column = tag.column - index_base_;
        std::size_t begin = 0;
        std::size_t end = 0;
        if (!FindDelimitedToken(lines[row], column, delimiters_, begin, end)) {
            throw doe::log::RuntimeError("extractor", instance_id, source,
                                         "column index out of range");
        }
        const std::string cell = Trim(lines[row].substr(begin, end - begin));

        const auto binding = output_bindings.find(tag.output);
        if (binding == output_bindings.end()) {
            throw doe::log::RuntimeError("extractor", instance_id, source,
                                         "output port \"" + tag.output +
                                             "\" has no response binding");
        }
        const ExtractorOutput* output_definition = nullptr;
        for (const auto& candidate : outputs_) {
            if (candidate.port == tag.output) {
                output_definition = &candidate;
                break;
            }
        }
        if (!output_definition) {
            throw doe::log::RuntimeError("extractor", instance_id, source,
                                         "tag references an undeclared output port \"" +
                                             tag.output + "\"");
        }
        if (output_definition->value_type != doe::config::ValueType::kReal) {
            throw doe::log::RuntimeError("extractor", instance_id, source,
                                         "unsupported value_type");
        }
        responses.emplace(binding->second, ParseRealValue(cell, instance_id));
    }
    return responses;
}

}  // namespace doe::runtime
