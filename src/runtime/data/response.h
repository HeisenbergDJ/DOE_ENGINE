/********************************************************************
 * "response.h" defines the runtime Response class.                *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#ifndef DOE_ENGINE_RUNTIME_DATA_RESPONSE_H_
#define DOE_ENGINE_RUNTIME_DATA_RESPONSE_H_

#include "config_types.h"
#include "responses_config.h"
#include "scalar_value.h"

#include <string>

namespace doe::runtime {

// A Response is the runtime instance of a config ResponseConfig. It keeps the
// declarative metadata and the value observed for one design point, and
// supports comparison so responses can be ordered and compared.
class Response {
public:
    explicit Response(const doe::config::ResponseConfig& config);

    const std::string& id() const noexcept;
    const std::string& name() const noexcept;
    const std::string& description() const noexcept;
    const std::string& unit() const noexcept;
    doe::config::ValueType value_type() const noexcept;
    const ScalarValue& value() const noexcept;
    void set_value(ScalarValue value);

    bool operator==(const Response& other) const;
    bool operator!=(const Response& other) const;
    bool operator<(const Response& other) const;

private:
    std::string id_;
    std::string name_;
    std::string description_;
    std::string unit_;
    doe::config::ValueType value_type_;
    ScalarValue value_;
};

inline bool operator>(const Response& lhs, const Response& rhs)
{
    return rhs < lhs;
}
inline bool operator<=(const Response& lhs, const Response& rhs)
{
    return !(rhs < lhs);
}
inline bool operator>=(const Response& lhs, const Response& rhs)
{
    return !(lhs < rhs);
}

}  // namespace doe::runtime

#endif  // DOE_ENGINE_RUNTIME_DATA_RESPONSE_H_
