/********************************************************************
 * "sampling_config.h" defines DOE sampling configuration.          *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#ifndef DOE_ENGINE_CONFIG_SAMPLING_CONFIG_H_
#define DOE_ENGINE_CONFIG_SAMPLING_CONFIG_H_

#include "config_types.h"

#include <cstddef>
#include <string>
#include <vector>

namespace doe::config {

struct FactorConfig {
    std::string variable;
    std::size_t levels = 0;
};

// Serialized by sampling.json. The first version supports only an equally
// spaced full-factorial design, with a separate level count for each variable.
struct SamplingConfig {
    SamplingType type = SamplingType::kFullFactorial;
    std::vector<FactorConfig> factors;
};

}  // namespace doe::config

#endif  // DOE_ENGINE_CONFIG_SAMPLING_CONFIG_H_
