/********************************************************************
 * "delimited_text.cc" provides delimited text helpers.           *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#include "delimited_text.h"

#include <string>
#include <utility>
#include <vector>

namespace doe::utils {

namespace {

bool MatchDelimiter(const std::string& line, std::size_t position,
                    const std::vector<std::string>& delimiters, std::size_t& length)
{
    for (const std::string& delimiter : delimiters) {
        if (delimiter.empty()) {
            continue;
        }
        if (line.compare(position, delimiter.size(), delimiter) == 0) {
            length = delimiter.size();
            return true;
        }
    }
    return false;
}

}  // namespace

bool FindDelimitedToken(const std::string& line, std::size_t column,
                        const std::vector<std::string>& delimiters, std::size_t& begin,
                        std::size_t& end)
{
    std::size_t current_column = 0;
    std::size_t position = 0;
    while (position < line.size()) {
        std::size_t length = 0;
        while (position < line.size() && MatchDelimiter(line, position, delimiters, length)) {
            position += length;
        }
        if (position >= line.size()) {
            break;
        }
        begin = position;
        while (position < line.size() &&
               !MatchDelimiter(line, position, delimiters, length)) {
            ++position;
        }
        end = position;
        if (current_column == column) {
            return true;
        }
        ++current_column;
    }
    return false;
}

std::vector<std::string> SplitTextLines(const std::string& text)
{
    std::vector<std::string> lines;
    std::size_t position = 0;
    while (position < text.size()) {
        const std::size_t newline = text.find('\n', position);
        std::string line = text.substr(
            position, newline == std::string::npos ? std::string::npos : newline - position);
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        lines.push_back(std::move(line));
        if (newline == std::string::npos) {
            break;
        }
        position = newline + 1;
    }
    return lines;
}

}  // namespace doe::utils
