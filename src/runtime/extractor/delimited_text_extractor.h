/********************************************************************
 * "delimited_text_extractor.h" declares the delimited_text type.  *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#ifndef DOE_ENGINE_RUNTIME_EXTRACTOR_DELIMITED_TEXT_EXTRACTOR_H_
#define DOE_ENGINE_RUNTIME_EXTRACTOR_DELIMITED_TEXT_EXTRACTOR_H_

#include "extractor.h"

#include <cstddef>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

namespace doe::runtime {

// Runtime form of one delimited extractor tag.
struct DelimitedExtractorTag {
    std::size_t row;
    std::size_t column;
    std::string output;
};

// Runtime form of one extractor output port.
struct ExtractorOutput {
    std::string port;
    doe::config::ValueType value_type;
};

// delimited_text: the reverse of a delimited_text Tagger. Splits the bound
// source file on delimiters and maps the cell at each (row, column) tag to its
// output-port response.
class DelimitedTextExtractor : public ResponseExtractor {
public:
    DelimitedTextExtractor(std::vector<std::string> input_ports, std::size_t index_base,
                           std::vector<std::string> delimiters,
                           std::vector<DelimitedExtractorTag> tags,
                           std::vector<ExtractorOutput> outputs);

    std::map<std::string, ScalarValue> Extract(
        const std::string& instance_id,
        const std::map<std::string, std::string>& output_bindings,
        const std::map<std::string, std::filesystem::path>& input_files) const override;

private:
    std::vector<std::string> input_ports_;
    std::size_t index_base_;
    std::vector<std::string> delimiters_;
    std::vector<DelimitedExtractorTag> tags_;
    std::vector<ExtractorOutput> outputs_;
};

}  // namespace doe::runtime

#endif  // DOE_ENGINE_RUNTIME_EXTRACTOR_DELIMITED_TEXT_EXTRACTOR_H_
