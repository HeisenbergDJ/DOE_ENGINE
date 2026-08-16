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

#include <cstddef>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

namespace doe::persistence {

// Optional interface that observes and records a DOE Engine run. DOE Engine
// must run correctly without any Persistence implementation; the Experiment
// coordinator calls these hooks at key points and never relies on their
// outcome.
//
// Design points are recorded lazily: a point row is written once when the point
// completes, with its final status and observed responses (single write per
// point), so very large designs do not require pre-populating the database and
// each process can write the points it ran.
class Persistence {
public:
    virtual ~Persistence() = default;

    virtual void RecordExperimentStart(const std::string& run_id,
                                       const std::string& config_hash) = 0;
    virtual void RecordExperimentEnd(const std::string& run_id, bool success) = 0;

    virtual void RecordApplicationRun(
        const std::string& run_id, std::size_t point_index, const std::string& application_id,
        const std::map<std::string, std::filesystem::path>& outputs) = 0;

    virtual void RecordPointOutcome(
        const std::string& run_id, std::size_t point_index,
        const doe::runtime::DesignPoint& point, bool success, const std::string& error_message,
        const std::map<std::string, doe::runtime::ScalarValue>& responses) = 0;

    // Returns the point indices that already have a terminal outcome (SUCCESS or
    // FAILED), used to skip completed points when resuming a run.
    virtual std::vector<std::size_t> TerminalPoints(const std::string& run_id) const = 0;

    // Returns the recorded config_hash of a run, or an empty string when the
    // run is unknown.
    virtual std::string ConfigHash(const std::string& run_id) const = 0;
};

}  // namespace doe::persistence

#endif  // DOE_ENGINE_PERSISTENCE_PERSISTENCE_H_
