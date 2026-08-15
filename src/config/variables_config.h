/********************************************************************
 * "variables_config.h" defines DOE design-variable configuration.  *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#ifndef DOE_ENGINE_CONFIG_VARIABLES_CONFIG_H_
#define DOE_ENGINE_CONFIG_VARIABLES_CONFIG_H_

#include "config_types.h"

#include <string>
#include <variant>
#include <vector>

namespace doe::config {

struct ContinuousDomainConfig {
    DomainType type = DomainType::kContinuous;
    double lower_bound = 0.0;
    double upper_bound = 0.0;
};

// A discrete domain is the explicit set of allowed values, for example the
// even integers {2, 4, 6} or the cities {北京, 上海, 成都}.
struct DiscreteDomainConfig {
    DomainType type = DomainType::kDiscrete;
    std::vector<ConfigScalar> values;
};

using DomainConfig = std::variant<ContinuousDomainConfig, DiscreteDomainConfig>;

struct VariableConfig {
    std::string id;
    std::string name;
    std::string description;
    std::string unit;
    ValueType value_type = ValueType::kReal;
    DomainConfig domain;
};

// Serialized by variables.json.
struct VariablesConfig {
    std::vector<VariableConfig> variables;
};

}  // namespace doe::config

#endif  // DOE_ENGINE_CONFIG_VARIABLES_CONFIG_H_
