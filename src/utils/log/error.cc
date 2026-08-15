/********************************************************************
 * "error.cc" provides the configuration loading error type.       *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#include "error.h"

#include <utility>

namespace doe::log {

ConfigError::ConfigError(std::filesystem::path source, std::string field, std::string message)
    : std::runtime_error(BuildMessage(source, field, message)),
      source_(std::move(source)),
      field_(std::move(field)) {}

ConfigError::ConfigError(std::string message)
    : ConfigError(std::filesystem::path(), "", std::move(message)) {}

const std::filesystem::path& ConfigError::source() const noexcept
{
    return source_;
}

const std::string& ConfigError::field() const noexcept
{
    return field_;
}

std::string ConfigError::BuildMessage(const std::filesystem::path& source,
                                      const std::string& field,
                                      const std::string& message)
{
    std::string result;
    if (!source.empty()) {
        result += source.string();
        if (!field.empty()) {
            result += '#';
            result += field;
        }
        result += ": ";
    }
    result += message;
    return result;
}

RuntimeError::RuntimeError(std::string stage, std::string object_id,
                           std::filesystem::path path, std::string message)
    : std::runtime_error(BuildMessage(stage, object_id, path, message)),
      stage_(std::move(stage)),
      object_id_(std::move(object_id)),
      path_(std::move(path)) {}

const std::string& RuntimeError::stage() const noexcept
{
    return stage_;
}

const std::string& RuntimeError::object_id() const noexcept
{
    return object_id_;
}

const std::filesystem::path& RuntimeError::path() const noexcept
{
    return path_;
}

std::string RuntimeError::BuildMessage(const std::string& stage,
                                       const std::string& object_id,
                                       const std::filesystem::path& path,
                                       const std::string& message)
{
    std::string result;
    if (!stage.empty()) {
        result += stage;
    }
    if (!object_id.empty()) {
        if (!result.empty()) {
            result += ' ';
        }
        result += '"';
        result += object_id;
        result += '"';
    }
    if (!path.empty()) {
        if (!result.empty()) {
            result += ' ';
        }
        result += '(';
        result += path.string();
        result += ')';
    }
    if (!result.empty()) {
        result += ": ";
    }
    result += message;
    return result;
}

}  // namespace doe::log
