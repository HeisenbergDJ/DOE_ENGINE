/********************************************************************
 * "response.cc" provides the runtime Response class.              *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#include "response.h"

#include <string>
#include <utility>

namespace doe::runtime {

Response::Response(const doe::config::ResponseConfig& config)
    : id_(config.id),
      name_(config.name),
      description_(config.description),
      unit_(config.unit),
      value_type_(config.value_type),
      value_(0.0)
{
}

const std::string& Response::id() const noexcept
{
    return id_;
}

const std::string& Response::name() const noexcept
{
    return name_;
}

const std::string& Response::description() const noexcept
{
    return description_;
}

const std::string& Response::unit() const noexcept
{
    return unit_;
}

doe::config::ValueType Response::value_type() const noexcept
{
    return value_type_;
}

const ScalarValue& Response::value() const noexcept
{
    return value_;
}

void Response::set_value(ScalarValue value)
{
    value_ = std::move(value);
}

bool Response::operator==(const Response& other) const
{
    return id_ == other.id_ && value_ == other.value_;
}

bool Response::operator!=(const Response& other) const
{
    return !(*this == other);
}

bool Response::operator<(const Response& other) const
{
    if (id_ != other.id_) {
        return id_ < other.id_;
    }
    return value_ < other.value_;
}

}  // namespace doe::runtime
