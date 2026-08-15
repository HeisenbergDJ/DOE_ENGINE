/********************************************************************
 * "experiment_config.h" defines the root Experiment configuration. *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#ifndef DOE_ENGINE_CONFIG_EXPERIMENT_CONFIG_H_
#define DOE_ENGINE_CONFIG_EXPERIMENT_CONFIG_H_

#include "config_types.h"

#include <string>

namespace doe::config {

// Serialized by experiment.json. This is the root configuration object passed
// to DOE Engine; the referenced files are loaded by the config layer.
struct ExperimentConfig {
    std::string format;
    int format_version = 0;
    std::string id;
    std::string name;
    std::string description;
    FileReference variables;
    FileReference responses;
    FileReference sampling;
    FileReference workflow;
    FileReference extractors;
};

}  // namespace doe::config

#endif  // DOE_ENGINE_CONFIG_EXPERIMENT_CONFIG_H_
