/********************************************************************
 * "responses_config.h" defines DOE response configuration.         *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#ifndef DOE_ENGINE_CONFIG_RESPONSES_CONFIG_H_
#define DOE_ENGINE_CONFIG_RESPONSES_CONFIG_H_

#include "config_types.h"

#include <string>
#include <vector>

namespace doe::config {

struct ResponseConfig {
    std::string id;
    std::string name;
    std::string description;
    std::string unit;
    ValueType value_type = ValueType::kReal;
};

// Serialized by responses.json. A Response is a DOE response variable, not an
// HTTP or process response.
struct ResponsesConfig {
    std::vector<ResponseConfig> responses;
};

}  // namespace doe::config

#endif  // DOE_ENGINE_CONFIG_RESPONSES_CONFIG_H_
