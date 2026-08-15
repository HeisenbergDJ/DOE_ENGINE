/********************************************************************
 * "error.h" defines the configuration loading error type.          *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#ifndef DOE_ENGINE_UTILS_LOG_ERROR_H_
#define DOE_ENGINE_UTILS_LOG_ERROR_H_

#include <filesystem>
#include <stdexcept>
#include <string>

namespace doe::log {

// Signals a configuration loading or validation failure. The message carries
// the source JSON file and, when a JSON field is involved, its location inside
// that file, for example "experiment.json#/variables/0/value_type: ...".
class ConfigError : public std::runtime_error {
public:
    ConfigError(std::filesystem::path source, std::string field, std::string message);
    explicit ConfigError(std::string message);

    const std::filesystem::path& source() const noexcept;
    const std::string& field() const noexcept;

private:
    static std::string BuildMessage(const std::filesystem::path& source,
                                    const std::string& field,
                                    const std::string& message);

    std::filesystem::path source_;
    std::string field_;
};

// Signals a runtime execution failure. The message carries the stage
// (input/process/output/...), the object id (Application port, variable id),
// and, when relevant, the file path that caused the failure.
class RuntimeError : public std::runtime_error {
public:
    RuntimeError(std::string stage, std::string object_id, std::filesystem::path path, std::string message);

    const std::string& stage() const noexcept;
    const std::string& object_id() const noexcept;
    const std::filesystem::path& path() const noexcept;

private:
    static std::string BuildMessage(const std::string& stage,
                                    const std::string& object_id,
                                    const std::filesystem::path& path,
                                    const std::string& message);

    std::string stage_;
    std::string object_id_;
    std::filesystem::path path_;
};

}  // namespace doe::log

#endif  // DOE_ENGINE_UTILS_LOG_ERROR_H_
