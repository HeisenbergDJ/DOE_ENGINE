/********************************************************************
 * "persistence.h" declares the optional persistence interface.    *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#ifndef DOE_ENGINE_PERSISTENCE_PERSISTENCE_H_
#define DOE_ENGINE_PERSISTENCE_PERSISTENCE_H_

#include "design_point.h"
#include "scalar_value.h"

#include <filesystem>
#include <map>
#include <string>

namespace doe::persistence {

// Optional interface that observes and records a DOE Engine run. DOE Engine
// must run correctly without any Persistence implementation; the Experiment
// coordinator calls these hooks at key points and never relies on their
// outcome.
class Persistence {
public:
    virtual ~Persistence() = default;

    virtual void RecordExperimentStart(const std::string& run_id) = 0;
    virtual void RecordExperimentEnd(const std::string& run_id, bool success) = 0;

    virtual void RecordDesignPoint(const std::string& run_id,
                                   const doe::runtime::DesignPoint& point) = 0;
    virtual void RecordApplicationRun(
        const std::string& run_id, std::size_t point_index, const std::string& application_id,
        const std::map<std::string, std::filesystem::path>& outputs) = 0;
    virtual void RecordResponses(const std::string& run_id, std::size_t point_index,
                                 const std::map<std::string, doe::runtime::ScalarValue>& responses) = 0;
};

}  // namespace doe::persistence

#endif  // DOE_ENGINE_PERSISTENCE_PERSISTENCE_H_
