/********************************************************************
 * "config.h" provides the complete configuration model interface. *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#ifndef DOE_ENGINE_CONFIG_CONFIG_H_
#define DOE_ENGINE_CONFIG_CONFIG_H_

// Public umbrella header for the complete DOE Engine configuration model. It
// also defines LoadedExperimentConfig, the aggregate produced after every
// referenced configuration file has been loaded.

#include "application_config.h"
#include "config_types.h"
#include "experiment_config.h"
#include "extractor_config.h"
#include "responses_config.h"
#include "sampling_config.h"
#include "variables_config.h"
#include "workflow_config.h"

#include <filesystem>
#include <map>
#include <string>

namespace doe::config {

// The complete, still purely declarative result of loading experiment.json and
// every referenced configuration file. Runtime objects are constructed from
// this aggregate only after semantic validation succeeds.
struct LoadedExperimentConfig {
    std::filesystem::path experiment_file;

    // SHA-256 of experiment.json and every referenced configuration file, in a
    // canonical order. A fingerprint of the exact configuration this run was
    // loaded from; used for reproducibility and to detect input drift.
    std::string config_hash;

    ExperimentConfig experiment;
    VariablesConfig variables;
    ResponsesConfig responses;
    SamplingConfig sampling;
    WorkflowConfig workflow;
    ExtractorsConfig extractors;

    // Keys are instance ids from workflow.json and extractors.json. This lets
    // multiple instances refer to the same definition without losing identity.
    std::map<std::string, ApplicationConfig> applications;
    std::map<std::string, ExtractorDefinitionConfig> extractor_definitions;
};

}  // namespace doe::config

#endif  // DOE_ENGINE_CONFIG_CONFIG_H_
