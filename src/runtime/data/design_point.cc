/********************************************************************
 * "design_point.cc" provides the runtime DesignPoint class.       *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#include "design_point.h"

#include <map>
#include <utility>
#include <vector>

namespace doe::runtime {

DesignPoint::DesignPoint(std::size_t index) : index_(index) {}

DesignPoint::DesignPoint(std::size_t index, std::vector<Variable> variables) : index_(index)
{
    for (auto& variable : variables) {
        variables_.emplace(variable.id(), std::move(variable));
    }
}

std::size_t DesignPoint::index() const noexcept
{
    return index_;
}

void DesignPoint::set_variable(Variable variable)
{
    variables_.insert_or_assign(variable.id(), std::move(variable));
}

void DesignPoint::set_response(Response response)
{
    responses_.insert_or_assign(response.id(), std::move(response));
}

const std::map<std::string, Variable>& DesignPoint::variables() const noexcept
{
    return variables_;
}

const std::map<std::string, Response>& DesignPoint::responses() const noexcept
{
    return responses_;
}

bool DesignPoint::success() const noexcept
{
    return success_;
}

void DesignPoint::set_success(bool success)
{
    success_ = success;
}

const std::string& DesignPoint::error() const noexcept
{
    return error_;
}

void DesignPoint::set_error(std::string error)
{
    error_ = std::move(error);
}

bool DesignPoint::operator==(const DesignPoint& other) const
{
    return variables_ == other.variables_;
}

bool DesignPoint::operator!=(const DesignPoint& other) const
{
    return !(*this == other);
}

bool DesignPoint::operator<(const DesignPoint& other) const
{
    return variables_ < other.variables_;
}

}  // namespace doe::runtime
