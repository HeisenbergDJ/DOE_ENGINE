/********************************************************************
 * "sha256.h" declares the SHA-256 digest helper.                  *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-16                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#ifndef DOE_ENGINE_UTILS_TOOLS_SHA256_H_
#define DOE_ENGINE_UTILS_TOOLS_SHA256_H_

#include <string>

namespace doe::utils {

// Returns the lowercase hexadecimal SHA-256 digest of the given bytes.
std::string Sha256Hex(const std::string& data);

}  // namespace doe::utils

#endif  // DOE_ENGINE_UTILS_TOOLS_SHA256_H_
