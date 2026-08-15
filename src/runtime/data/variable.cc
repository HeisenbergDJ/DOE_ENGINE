/********************************************************************
 * "variable.cc" provides the runtime Variable class.              *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#include "variable.h"

#include <string>
#include <utility>
#include <vector>

namespace doe::runtime {

namespace {

ScalarValue InitialValue(const doe::config::DomainConfig& domain)
{
    if (std::holds_alternative<doe::config::ContinuousDomainConfig>(domain)) {
        return std::get<doe::config::ContinuousDomainConfig>(domain).lower_bound;
    }
    const std::vector<doe::config::ConfigScalar>& values =
        std::get<doe::config::DiscreteDomainConfig>(domain).values;
    if (values.empty()) {
        return 0.0;
    }
    return FromConfigScalar(values.front());
}

}  // namespace

Variable::Variable(const doe::config::VariableConfig& config)
    : id_(config.id),
      name_(config.name),
      description_(config.description),
      unit_(config.unit),
      value_type_(config.value_type),
      value_(InitialValue(config.domain))
{
}

const std::string& Variable::id() const noexcept
{
    return id_;
}

const std::string& Variable::name() const noexcept
{
    return name_;
}

const std::string& Variable::description() const noexcept
{
    return description_;
}

const std::string& Variable::unit() const noexcept
{
    return unit_;
}

doe::config::ValueType Variable::value_type() const noexcept
{
    return value_type_;
}

const ScalarValue& Variable::value() const noexcept
{
    return value_;
}

void Variable::set_value(ScalarValue value)
{
    value_ = std::move(value);
}

bool Variable::operator==(const Variable& other) const
{
    return id_ == other.id_ && value_ == other.value_;
}

bool Variable::operator!=(const Variable& other) const
{
    return !(*this == other);
}

bool Variable::operator<(const Variable& other) const
{
    if (id_ != other.id_) {
        return id_ < other.id_;
    }
    return value_ < other.value_;
}

}  // namespace doe::runtime
