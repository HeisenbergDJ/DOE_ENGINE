/********************************************************************
 * "tagger.cc" provides runtime tagger implementations.           *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#include "tagger.h"

#include "delimited_text.h"
#include "error.h"

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace doe::runtime {

using doe::log::RuntimeError;
using doe::utils::FindDelimitedToken;

std::unique_ptr<Tagger> MakeTagger(const doe::config::TaggerConfig& config)
{
    if (std::holds_alternative<doe::config::PlaceholderTextTaggerConfig>(config)) {
        const doe::config::PlaceholderTextTaggerConfig& source =
            std::get<doe::config::PlaceholderTextTaggerConfig>(config);
        std::vector<PlaceholderTag> tags;
        tags.reserve(source.tags.size());
        for (const auto& tag : source.tags) {
            tags.push_back({tag.placeholder, tag.variable});
        }
        return std::make_unique<PlaceholderTextTagger>(std::move(tags));
    }
    const doe::config::DelimitedTextTaggerConfig& source =
        std::get<doe::config::DelimitedTextTaggerConfig>(config);
    std::vector<DelimitedTag> tags;
    tags.reserve(source.tags.size());
    for (const auto& tag : source.tags) {
        tags.push_back({tag.row, tag.column, tag.variable});
    }
    return std::make_unique<DelimitedTextTagger>(source.index_base, source.delimiters,
                                                 source.value_format.notation,
                                                 source.value_format.precision, std::move(tags));
}

std::string FormatScalar(const ScalarValue& value, doe::config::ValueNotation notation,
                         int precision)
{
    if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    }
    if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    }
    const double number = std::get<double>(value);
    const char* spec = "%.*g";
    if (notation == doe::config::ValueNotation::kScientific) {
        spec = "%.*e";
    } else if (notation == doe::config::ValueNotation::kFixed) {
        spec = "%.*f";
    }
    char buffer[256];
    const int written = std::snprintf(buffer, sizeof buffer, spec, precision, number);
    if (written < 0 || static_cast<std::size_t>(written) >= sizeof buffer) {
        return std::to_string(number);
    }
    return std::string(buffer);
}

PlaceholderTextTagger::PlaceholderTextTagger(std::vector<PlaceholderTag> tags)
    : tags_(std::move(tags))
{
}

std::string PlaceholderTextTagger::Apply(const std::string& text,
                                         const std::map<std::string, ScalarValue>& values) const
{
    std::string result = text;
    for (const auto& tag : tags_) {
        auto found = values.find(tag.variable);
        if (found == values.end()) {
            throw RuntimeError("tagger", tag.variable, std::filesystem::path(),
                               "missing value for tagged variable");
        }
        const std::string replacement =
            FormatScalar(found->second, doe::config::ValueNotation::kGeneral, 17);
        std::string::size_type position = 0;
        while ((position = result.find(tag.placeholder, position)) != std::string::npos) {
            result.replace(position, tag.placeholder.size(), replacement);
            position += replacement.size();
        }
    }
    return result;
}

DelimitedTextTagger::DelimitedTextTagger(std::size_t index_base,
                                         std::vector<std::string> delimiters,
                                         doe::config::ValueNotation notation, int precision,
                                         std::vector<DelimitedTag> tags)
    : index_base_(index_base),
      delimiters_(std::move(delimiters)),
      notation_(notation),
      precision_(precision),
      tags_(std::move(tags))
{
}

std::string DelimitedTextTagger::Apply(const std::string& text,
                                       const std::map<std::string, ScalarValue>& values) const
{
    std::string result;
    std::size_t position = 0;
    std::size_t line_index = 0;
    while (position < text.size()) {
        const std::size_t newline = text.find('\n', position);
        std::string line = text.substr(
            position, newline == std::string::npos ? std::string::npos : newline - position);
        const bool had_cr = !line.empty() && line.back() == '\r';
        if (had_cr) {
            line.pop_back();
        }
        line = ApplyToLine(line, line_index, values);
        result += line;
        if (had_cr) {
            result += '\r';
        }
        if (newline == std::string::npos) {
            break;
        }
        result += '\n';
        position = newline + 1;
        ++line_index;
    }
    return result;
}

std::string DelimitedTextTagger::ApplyToLine(
    const std::string& line, std::size_t line_index,
    const std::map<std::string, ScalarValue>& values) const
{
    std::vector<const DelimitedTag*> line_tags;
    for (const auto& tag : tags_) {
        if (tag.row < index_base_) {
            throw RuntimeError("tagger", tag.variable, std::filesystem::path(),
                               "row index below index_base");
        }
        const std::size_t actual_row = tag.row - index_base_;
        if (actual_row == line_index) {
            line_tags.push_back(&tag);
        }
    }
    std::sort(line_tags.begin(), line_tags.end(),
              [](const DelimitedTag* lhs, const DelimitedTag* rhs) {
                  return lhs->column < rhs->column;
              });

    std::string result = line;
    for (const DelimitedTag* tag : line_tags) {
        if (tag->column < index_base_) {
            throw RuntimeError("tagger", tag->variable, std::filesystem::path(),
                               "column index below index_base");
        }
        const std::size_t column = tag->column - index_base_;
        std::size_t begin = 0;
        std::size_t end = 0;
        if (!FindDelimitedToken(result, column, delimiters_, begin, end)) {
            throw RuntimeError("tagger", tag->variable, std::filesystem::path(),
                               "column index out of range");
        }
        auto found = values.find(tag->variable);
        if (found == values.end()) {
            throw RuntimeError("tagger", tag->variable, std::filesystem::path(),
                               "missing value for tagged variable");
        }
        const std::string replacement = FormatScalar(found->second, notation_, precision_);
        result.replace(begin, end - begin, replacement);
    }
    return result;
}

}  // namespace doe::runtime
