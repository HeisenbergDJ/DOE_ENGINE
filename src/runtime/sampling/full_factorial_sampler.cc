/********************************************************************
 * "full_factorial_sampler.cc" provides the full-factorial sampler. *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#include "full_factorial_sampler.h"

#include "error.h"
#include "scalar_value.h"

#include <filesystem>
#include <string>
#include <utility>
#include <vector>

namespace doe::runtime {

namespace {

using doe::log::RuntimeError;

const doe::config::VariableConfig* FindVariable(const doe::config::VariablesConfig& variables,
                                                const std::string& variable_id)
{
    for (const auto& variable : variables.variables) {
        if (variable.id == variable_id) {
            return &variable;
        }
    }
    return nullptr;
}

// Equally spaced levels between the domain bounds, always including both
// bounds. levels is guaranteed to be at least 2 by the caller.
std::vector<ScalarValue> GenerateLevels(const doe::config::ContinuousDomainConfig& domain,
                                        std::size_t levels)
{
    std::vector<ScalarValue> values;
    values.reserve(levels);
    for (std::size_t i = 0; i < levels; ++i) {
        const double value =
            domain.lower_bound +
            static_cast<double>(i) * (domain.upper_bound - domain.lower_bound) /
                static_cast<double>(levels - 1);
        values.push_back(value);
    }
    return values;
}

}  // namespace

FullFactorialSampler::FullFactorialSampler(const doe::config::VariablesConfig& variables,
                                           const doe::config::SamplingConfig& sampling)
{
    if (sampling.type != doe::config::SamplingType::kFullFactorial) {
        throw RuntimeError("sampling", "", std::filesystem::path(),
                           "unsupported sampling type");
    }

    variables_.reserve(sampling.factors.size());
    levels_.reserve(sampling.factors.size());
    for (const auto& factor : sampling.factors) {
        const doe::config::VariableConfig* config = FindVariable(variables, factor.variable);
        if (!config) {
            throw RuntimeError("sampling", factor.variable, std::filesystem::path(),
                               "factor references an undeclared variable");
        }
        variables_.emplace_back(*config);
        if (std::holds_alternative<doe::config::ContinuousDomainConfig>(config->domain)) {
            if (factor.levels < 2) {
                throw RuntimeError("sampling", factor.variable, std::filesystem::path(),
                                   "factor levels must be at least 2");
            }
            levels_.push_back(GenerateLevels(
                std::get<doe::config::ContinuousDomainConfig>(config->domain), factor.levels));
        } else {
            const std::vector<doe::config::ConfigScalar>& values =
                std::get<doe::config::DiscreteDomainConfig>(config->domain).values;
            if (values.empty()) {
                throw RuntimeError("sampling", factor.variable, std::filesystem::path(),
                                   "discrete domain has no values");
            }
            std::vector<ScalarValue> levels;
            levels.reserve(values.size());
            for (const auto& value : values) {
                levels.push_back(FromConfigScalar(value));
            }
            levels_.push_back(std::move(levels));
        }
    }
}

std::vector<DesignPoint> FullFactorialSampler::Generate() const
{
    if (levels_.empty()) {
        std::vector<DesignPoint> single;
        single.emplace_back(0);
        return single;
    }

    std::vector<DesignPoint> points;
    std::vector<std::size_t> counters(levels_.size(), 0);
    bool done = false;
    std::size_t index = 0;
    while (!done) {
        DesignPoint point(index);
        for (std::size_t factor = 0; factor < levels_.size(); ++factor) {
            Variable variable = variables_[factor];
            variable.set_value(levels_[factor][counters[factor]]);
            point.set_variable(std::move(variable));
        }
        points.push_back(std::move(point));
        ++index;

        std::size_t factor = levels_.size();
        while (factor > 0) {
            --factor;
            if (counters[factor] + 1 < levels_[factor].size()) {
                ++counters[factor];
                break;
            }
            counters[factor] = 0;
            if (factor == 0) {
                done = true;
            }
        }
    }
    return points;
}

}  // namespace doe::runtime
