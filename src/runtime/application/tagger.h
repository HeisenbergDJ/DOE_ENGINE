/********************************************************************
 * "tagger.h" declares runtime tagger implementations.             *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#ifndef DOE_ENGINE_RUNTIME_APPLICATION_TAGGER_H_
#define DOE_ENGINE_RUNTIME_APPLICATION_TAGGER_H_

#include "application_config.h"
#include "scalar_value.h"

#include <cstddef>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace doe::runtime {

// Runtime form of one placeholder tag.
struct PlaceholderTag {
    std::string placeholder;
    std::string variable;
};

// Runtime form of one delimited tag.
struct DelimitedTag {
    std::size_t row;
    std::size_t column;
    std::string variable;
};

// Substitutes design-variable values into a template. Implementations map the
// two serialized tagger types (placeholder_text, delimited_text).
class Tagger {
public:
    virtual ~Tagger() = default;

    virtual std::string Apply(const std::string& text,
                              const std::map<std::string, ScalarValue>& values) const = 0;
};

std::unique_ptr<Tagger> MakeTagger(const doe::config::TaggerConfig& config);

// Replaces every occurrence of each configured placeholder with the formatted
// value of its variable.
class PlaceholderTextTagger : public Tagger {
public:
    explicit PlaceholderTextTagger(std::vector<PlaceholderTag> tags);

    std::string Apply(const std::string& text,
                      const std::map<std::string, ScalarValue>& values) const override;

private:
    std::vector<PlaceholderTag> tags_;
};

// Replaces, for each configured tag, the token at (row, column) of the text
// with the formatted value of its variable. Rows are lines; columns are tokens
// split on any configured delimiter, with consecutive delimiters collapsed.
// index_base is applied to both row and column indexes.
class DelimitedTextTagger : public Tagger {
public:
    DelimitedTextTagger(std::size_t index_base, std::vector<std::string> delimiters,
                        doe::config::ValueNotation notation, int precision,
                        std::vector<DelimitedTag> tags);

    std::string Apply(const std::string& text,
                      const std::map<std::string, ScalarValue>& values) const override;

private:
    std::string ApplyToLine(const std::string& line, std::size_t line_index,
                            const std::map<std::string, ScalarValue>& values) const;

    std::size_t index_base_;
    std::vector<std::string> delimiters_;
    doe::config::ValueNotation notation_;
    int precision_;
    std::vector<DelimitedTag> tags_;
};

// Formats a runtime scalar value with the given notation and precision. Int and
// text values are passed through unchanged.
std::string FormatScalar(const ScalarValue& value, doe::config::ValueNotation notation,
                         int precision);

}  // namespace doe::runtime

#endif  // DOE_ENGINE_RUNTIME_APPLICATION_TAGGER_H_
