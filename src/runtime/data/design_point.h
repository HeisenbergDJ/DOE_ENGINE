/********************************************************************
 * "design_point.h" defines the runtime DesignPoint class.        *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#ifndef DOE_ENGINE_RUNTIME_DATA_DESIGN_POINT_H_
#define DOE_ENGINE_RUNTIME_DATA_DESIGN_POINT_H_

#include "response.h"
#include "variable.h"

#include <cstddef>
#include <map>
#include <string>
#include <vector>

namespace doe::runtime {

// A DesignPoint is one trial in a DOE experiment: a fixed combination of
// design-variable values (the inputs produced by sampling) together with the
// response values observed after the Workflow and Extractors have run. It is
// created at runtime, never deserialized from a Config file. The index orders
// the trial within a run ({design_point_dir} = {run_dir}/DESIGN_POINT_<index>).
class DesignPoint {
public:
    explicit DesignPoint(std::size_t index);
    DesignPoint(std::size_t index, std::vector<Variable> variables);

    std::size_t index() const noexcept;

    void set_variable(Variable variable);
    void set_response(Response response);

    const std::map<std::string, Variable>& variables() const noexcept;
    const std::map<std::string, Response>& responses() const noexcept;

    // Outcome of running this trial. A freshly generated design point has not
    // succeeded yet; the Experiment coordinator records the result after the
    // Workflow and Extractors have run.
    bool success() const noexcept;
    void set_success(bool success);
    const std::string& error() const noexcept;
    void set_error(std::string error);

    // Two design points are equal when their input variable values match,
    // which is what identifies a trial; the index is a sequence number.
    bool operator==(const DesignPoint& other) const;
    bool operator!=(const DesignPoint& other) const;
    bool operator<(const DesignPoint& other) const;

private:
    std::size_t index_;
    std::map<std::string, Variable> variables_;
    std::map<std::string, Response> responses_;
    bool success_ = false;
    std::string error_;
};

inline bool operator>(const DesignPoint& lhs, const DesignPoint& rhs)
{
    return rhs < lhs;
}
inline bool operator<=(const DesignPoint& lhs, const DesignPoint& rhs)
{
    return !(rhs < lhs);
}
inline bool operator>=(const DesignPoint& lhs, const DesignPoint& rhs)
{
    return !(lhs < rhs);
}

}  // namespace doe::runtime

#endif  // DOE_ENGINE_RUNTIME_DATA_DESIGN_POINT_H_
