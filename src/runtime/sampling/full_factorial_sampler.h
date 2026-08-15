/********************************************************************
 * "full_factorial_sampler.h" declares the full-factorial sampler. *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#ifndef DOE_ENGINE_RUNTIME_SAMPLING_FULL_FACTORIAL_SAMPLER_H_
#define DOE_ENGINE_RUNTIME_SAMPLING_FULL_FACTORIAL_SAMPLER_H_

#include "sampling.h"
#include "variable.h"

#include <cstddef>
#include <vector>

namespace doe::runtime {

// Full-factorial design: the cartesian product of every factor's levels, with
// the first factor varying slowest and the last factor varying fastest. The
// constructor reads the Config once and pre-compiles each factor into a runtime
// Variable template (variables_) and its level values (levels_); Generate()
// only enumerates the cartesian product.
class FullFactorialSampler : public Sampler {
public:
    FullFactorialSampler(const doe::config::VariablesConfig& variables,
                         const doe::config::SamplingConfig& sampling);

    std::vector<DesignPoint> Generate() const override;

private:
    std::vector<Variable> variables_;
    std::vector<std::vector<ScalarValue>> levels_;
};

}  // namespace doe::runtime

#endif  // DOE_ENGINE_RUNTIME_SAMPLING_FULL_FACTORIAL_SAMPLER_H_
