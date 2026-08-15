/********************************************************************
 * "scalar_value.h" defines the shared runtime scalar value.       *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#ifndef DOE_ENGINE_RUNTIME_DATA_SCALAR_VALUE_H_
#define DOE_ENGINE_RUNTIME_DATA_SCALAR_VALUE_H_

#include "config_types.h"

#include <string>
#include <variant>

namespace doe::runtime {

// The scalar value carried by runtime Variable and Response objects. The first
// DOE Engine version uses real values only; integer and text alternatives are
// provided so the same value type can serve later value types without
// changing callers.
using ScalarValue = std::variant<int, double, std::string>;

// Converts a config-side ConfigScalar into the runtime ScalarValue.
ScalarValue FromConfigScalar(const doe::config::ConfigScalar& value);

// Relational operators on ScalarValue. Numbers compare numerically (an int is
// widened to double); a number and a string are ordered so numbers precede
// strings, giving a strict total order.
bool operator==(const ScalarValue& lhs, const ScalarValue& rhs);
bool operator!=(const ScalarValue& lhs, const ScalarValue& rhs);
bool operator<(const ScalarValue& lhs, const ScalarValue& rhs);
inline bool operator>(const ScalarValue& lhs, const ScalarValue& rhs)
{
    return rhs < lhs;
}
inline bool operator<=(const ScalarValue& lhs, const ScalarValue& rhs)
{
    return !(rhs < lhs);
}
inline bool operator>=(const ScalarValue& lhs, const ScalarValue& rhs)
{
    return !(lhs < rhs);
}

}  // namespace doe::runtime

#endif  // DOE_ENGINE_RUNTIME_DATA_SCALAR_VALUE_H_
