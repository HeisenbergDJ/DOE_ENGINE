/********************************************************************
 * "config_types.h" defines shared configuration value types.       *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#ifndef DOE_ENGINE_CONFIG_CONFIG_TYPES_H_
#define DOE_ENGINE_CONFIG_CONFIG_TYPES_H_

#include <filesystem>
#include <string>
#include <variant>

namespace doe::config {

// Only the types implemented by the first DOE Engine version are listed here.
// New JSON spellings must be added deliberately instead of silently falling
// back to an existing enum value.
enum class ValueType { kReal, kInteger, kString };
enum class DomainType { kContinuous, kDiscrete };
enum class SamplingType { kFullFactorial };
enum class ApplicationType { kCommand };
enum class ApplicationInputMode { kCopy, kTaggedCopy };
enum class ArtifactType { kFile };
enum class ValueNotation { kScientific, kFixed, kGeneral };

// A config-side scalar value used by discrete domain entries. It mirrors the
// runtime ScalarValue; the two layers convert directly.
using ConfigScalar = std::variant<int, double, std::string>;

// A referenced JSON file is resolved relative to the JSON file containing the
// reference. It is not resolved relative to the process working directory.
struct FileReference {
    std::filesystem::path file;
};

}  // namespace doe::config

#endif  // DOE_ENGINE_CONFIG_CONFIG_TYPES_H_
