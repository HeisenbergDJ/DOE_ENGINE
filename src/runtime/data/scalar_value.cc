/********************************************************************
 * "scalar_value.cc" provides the shared runtime scalar value.     *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#include "scalar_value.h"

#include <string>
#include <variant>

namespace doe::runtime {

namespace {

bool IsNumeric(const ScalarValue& value)
{
    return std::holds_alternative<int>(value) || std::holds_alternative<double>(value);
}

double AsDouble(const ScalarValue& value)
{
    if (std::holds_alternative<int>(value)) {
        return static_cast<double>(std::get<int>(value));
    }
    return std::get<double>(value);
}

int CompareValues(const ScalarValue& lhs, const ScalarValue& rhs)
{
    const bool lhs_numeric = IsNumeric(lhs);
    const bool rhs_numeric = IsNumeric(rhs);
    if (lhs_numeric && rhs_numeric) {
        const double lhs_number = AsDouble(lhs);
        const double rhs_number = AsDouble(rhs);
        if (lhs_number < rhs_number) {
            return -1;
        }
        if (lhs_number > rhs_number) {
            return 1;
        }
        return 0;
    }
    if (lhs_numeric != rhs_numeric) {
        return lhs_numeric ? -1 : 1;
    }
    const std::string& lhs_string = std::get<std::string>(lhs);
    const std::string& rhs_string = std::get<std::string>(rhs);
    if (lhs_string < rhs_string) {
        return -1;
    }
    if (lhs_string > rhs_string) {
        return 1;
    }
    return 0;
}

}  // namespace

bool operator==(const ScalarValue& lhs, const ScalarValue& rhs)
{
    return CompareValues(lhs, rhs) == 0;
}

bool operator!=(const ScalarValue& lhs, const ScalarValue& rhs)
{
    return !(lhs == rhs);
}

bool operator<(const ScalarValue& lhs, const ScalarValue& rhs)
{
    return CompareValues(lhs, rhs) < 0;
}

ScalarValue FromConfigScalar(const doe::config::ConfigScalar& value)
{
    if (std::holds_alternative<int>(value)) {
        return std::get<int>(value);
    }
    if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    }
    return std::get<std::string>(value);
}

}  // namespace doe::runtime
