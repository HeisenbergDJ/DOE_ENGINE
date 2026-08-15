/********************************************************************
 * "workflow_config.h" defines Application Workflow configuration.  *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#ifndef DOE_ENGINE_CONFIG_WORKFLOW_CONFIG_H_
#define DOE_ENGINE_CONFIG_WORKFLOW_CONFIG_H_

#include "config_types.h"

#include <string>
#include <vector>

namespace doe::config {

struct ApplicationInstanceConfig {
    std::string id;
    FileReference configuration;
};

// A Workflow endpoint is identified by an Application instance id and one of
// that instance's logical ports. It never stores a physical file path.
struct WorkflowEndpointConfig {
    std::string application;
    std::string port;
};

struct WorkflowConnectionConfig {
    WorkflowEndpointConfig from;
    WorkflowEndpointConfig to;
};

// Serialized by workflow.json. Extractors are deliberately not Workflow nodes
// in the first implementation; they run after the Application Workflow.
struct WorkflowConfig {
    std::vector<ApplicationInstanceConfig> applications;
    std::vector<WorkflowConnectionConfig> connections;
};

}  // namespace doe::config

#endif  // DOE_ENGINE_CONFIG_WORKFLOW_CONFIG_H_
