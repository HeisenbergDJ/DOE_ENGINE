/********************************************************************
 * "variable.h" defines the runtime Variable class.               *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#ifndef DOE_ENGINE_RUNTIME_DATA_VARIABLE_H_
#define DOE_ENGINE_RUNTIME_DATA_VARIABLE_H_

#include "config_types.h"
#include "scalar_value.h"
#include "variables_config.h"

#include <string>

namespace doe::runtime {

// A Variable is the runtime instance of a config VariableConfig. It keeps the
// declarative metadata and the current value, and supports comparison so
// design points can be ordered and deduplicated.
class Variable {
public:
    explicit Variable(const doe::config::VariableConfig& config);

    const std::string& id() const noexcept;
    const std::string& name() const noexcept;
    const std::string& description() const noexcept;
    const std::string& unit() const noexcept;
    doe::config::ValueType value_type() const noexcept;
    const ScalarValue& value() const noexcept;
    void set_value(ScalarValue value);

    bool operator==(const Variable& other) const;
    bool operator!=(const Variable& other) const;
    bool operator<(const Variable& other) const;

private:
    std::string id_;
    std::string name_;
    std::string description_;
    std::string unit_;
    doe::config::ValueType value_type_;
    ScalarValue value_;
};

inline bool operator>(const Variable& lhs, const Variable& rhs)
{
    return rhs < lhs;
}
inline bool operator<=(const Variable& lhs, const Variable& rhs)
{
    return !(rhs < lhs);
}
inline bool operator>=(const Variable& lhs, const Variable& rhs)
{
    return !(lhs < rhs);
}

}  // namespace doe::runtime

#endif  // DOE_ENGINE_RUNTIME_DATA_VARIABLE_H_
