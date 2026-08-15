/********************************************************************
 * "extractor_util.h" declares shared extractor helpers.          *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#ifndef DOE_ENGINE_RUNTIME_EXTRACTOR_EXTRACTOR_UTIL_H_
#define DOE_ENGINE_RUNTIME_EXTRACTOR_EXTRACTOR_UTIL_H_

#include "scalar_value.h"

#include <filesystem>
#include <string>

namespace doe::runtime {

// Reads a whole text file.
std::string ReadTextFile(const std::filesystem::path& path);

// Strips leading and trailing whitespace.
std::string Trim(const std::string& text);

// Parses a real value, wrapping parse failures as doe::log::RuntimeError.
ScalarValue ParseRealValue(const std::string& text, const std::string& object_id);

}  // namespace doe::runtime

#endif  // DOE_ENGINE_RUNTIME_EXTRACTOR_EXTRACTOR_UTIL_H_
