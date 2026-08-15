/********************************************************************
 * "workflow.cc" provides the Application Workflow runner.         *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#include "workflow.h"

#include "error.h"

#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace doe::runtime {

namespace {

using doe::log::RuntimeError;

// Runs every declared Application instance in declaration order for one design
// point. Connections are recorded by the config but are not applied in the
// first version because the current Application input model has no input ports
// to bind.
class SequentialWorkflowRunner : public WorkflowRunner {
public:
    SequentialWorkflowRunner(const doe::config::WorkflowConfig& workflow,
                             std::map<std::string, Application> applications)
        : applications_(std::move(applications))
    {
        order_.reserve(workflow.applications.size());
        for (const auto& instance : workflow.applications) {
            if (applications_.find(instance.id) == applications_.end()) {
                throw RuntimeError("workflow", instance.id, std::filesystem::path(),
                                   "no Application instance with this id");
            }
            order_.push_back(instance.id);
        }
    }

    std::map<std::string, std::map<std::string, std::filesystem::path>> Run(
        const std::map<std::string, ScalarValue>& variable_values) const override
    {
        std::map<std::string, std::map<std::string, std::filesystem::path>> outputs;
        for (const std::string& id : order_) {
            const auto app = applications_.find(id);
            outputs.emplace(id, app->second.Run(variable_values));
        }
        return outputs;
    }

private:
    std::vector<std::string> order_;
    std::map<std::string, Application> applications_;
};

}  // namespace

std::unique_ptr<WorkflowRunner> MakeWorkflowRunner(
    const doe::config::WorkflowConfig& workflow,
    std::map<std::string, Application> applications)
{
    return std::make_unique<SequentialWorkflowRunner>(workflow, std::move(applications));
}

}  // namespace doe::runtime
