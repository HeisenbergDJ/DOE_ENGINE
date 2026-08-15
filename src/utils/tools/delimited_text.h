/********************************************************************
 * "delimited_text.h" declares delimited text helpers.            *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#ifndef DOE_ENGINE_UTILS_TOOLS_DELIMITED_TEXT_H_
#define DOE_ENGINE_UTILS_TOOLS_DELIMITED_TEXT_H_

#include <cstddef>
#include <string>
#include <vector>

namespace doe::utils {

// Finds the token at the zero-based column of a line. Tokens are maximal runs
// of non-delimiter characters; consecutive and leading delimiters collapse.
// Returns false when the column is out of range.
bool FindDelimitedToken(const std::string& line, std::size_t column,
                        const std::vector<std::string>& delimiters,
                        std::size_t& begin, std::size_t& end);

// Splits text into lines, stripping a trailing '\r' and dropping the '\n'.
std::vector<std::string> SplitTextLines(const std::string& text);

}  // namespace doe::utils

#endif  // DOE_ENGINE_UTILS_TOOLS_DELIMITED_TEXT_H_
