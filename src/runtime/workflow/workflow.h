/********************************************************************
 * "workflow.h" declares the Application Workflow runner.          *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#ifndef DOE_ENGINE_RUNTIME_WORKFLOW_WORKFLOW_H_
#define DOE_ENGINE_RUNTIME_WORKFLOW_WORKFLOW_H_

#include "application.h"
#include "scalar_value.h"
#include "workflow_config.h"

#include <filesystem>
#include <map>
#include <memory>
#include <string>

namespace doe::runtime {

// Runs the Application instances of one design point and returns, per
// Application instance id, the logical output port to physical file path
// mapping. A WorkflowRunner is constructed with the workflow and the runtime
// Applications it owns; Run() needs no further input. The first version runs
// the declared Applications serially; binding upstream outputs to downstream
// inputs through connections is a later refinement.
class WorkflowRunner {
public:
    virtual ~WorkflowRunner() = default;

    virtual std::map<std::string, std::map<std::string, std::filesystem::path>> Run(
        const std::map<std::string, ScalarValue>& variable_values) const = 0;
};

// Factory for WorkflowRunner implementations. Takes ownership of the runtime
// Applications.
std::unique_ptr<WorkflowRunner> MakeWorkflowRunner(
    const doe::config::WorkflowConfig& workflow,
    std::map<std::string, Application> applications);

}  // namespace doe::runtime

#endif  // DOE_ENGINE_RUNTIME_WORKFLOW_WORKFLOW_H_
