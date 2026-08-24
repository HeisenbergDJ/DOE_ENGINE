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

// Engine run-policy enums, serialized by engine_cfg.json.
//
// ArtifactStrategy decides the disposition of ALL files produced for a design
// point (input templates, intermediates and outputs): none keeps only the
// database; keep_required keeps the required outputs; keep_all keeps every
// file; tar packs each tar_batch_size design point directories into one
// archive and removes the originals.
enum class ArtifactStrategy { kNone, kKeepRequired, kKeepAll, kTar };
enum class WorkingDirectoryMode { kShared, kScratch };
enum class LogLevel { kInfo, kWarn, kError };
enum class SchedulerMode { kStatic, kDynamic };

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
