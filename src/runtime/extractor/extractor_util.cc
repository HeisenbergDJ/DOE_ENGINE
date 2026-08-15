/********************************************************************
 * "extractor_util.cc" provides shared extractor helpers.         *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#include "extractor_util.h"

#include "error.h"

#include <cctype>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace doe::runtime {

std::string ReadTextFile(const std::filesystem::path& path)
{
    std::ifstream stream(path, std::ios::binary);
    if (!stream) {
        throw doe::log::RuntimeError("extractor", "", path, "failed to open the result file");
    }
    std::ostringstream buffer;
    buffer << stream.rdbuf();
    return buffer.str();
}

std::string Trim(const std::string& text)
{
    std::size_t begin = 0;
    while (begin < text.size() && std::isspace(static_cast<unsigned char>(text[begin]))) {
        ++begin;
    }
    std::size_t end = text.size();
    while (end > begin && std::isspace(static_cast<unsigned char>(text[end - 1]))) {
        --end;
    }
    return text.substr(begin, end - begin);
}

ScalarValue ParseRealValue(const std::string& text, const std::string& object_id)
{
    try {
        return std::stod(text);
    } catch (const std::exception&) {
        throw doe::log::RuntimeError("extractor", object_id, std::filesystem::path(),
                                     "invalid real value \"" + text + "\"");
    }
}

}  // namespace doe::runtime
