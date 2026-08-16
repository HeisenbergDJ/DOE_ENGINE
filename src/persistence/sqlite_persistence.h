/********************************************************************
 * "sqlite_persistence.h" declares the SQLite persistence impl.    *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-16                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#ifndef DOE_ENGINE_PERSISTENCE_SQLITE_PERSISTENCE_H_
#define DOE_ENGINE_PERSISTENCE_SQLITE_PERSISTENCE_H_

#include "design_point.h"
#include "persistence.h"
#include "scalar_value.h"

#include <cstddef>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

struct sqlite3;

namespace doe::persistence {

// Persists a DOE Engine run into a SQLite database. Tables (runs,
// design_points, application_runs) are created on open. Design points are
// written once when they complete (lazy insert), so concurrent MPI processes
// each write their own points; WAL mode and a busy timeout keep concurrent
// writers working.
class SqlitePersistence : public Persistence {
public:
    explicit SqlitePersistence(const std::filesystem::path& db_path);
    ~SqlitePersistence() override;

    void RecordExperimentStart(const std::string& run_id,
                               const std::string& config_hash) override;
    void RecordExperimentEnd(const std::string& run_id, bool success) override;
    void RecordApplicationRun(
        const std::string& run_id, std::size_t point_index, const std::string& application_id,
        const std::map<std::string, std::filesystem::path>& outputs) override;
    void RecordPointOutcome(
        const std::string& run_id, std::size_t point_index,
        const doe::runtime::DesignPoint& point, bool success, const std::string& error_message,
        const std::map<std::string, doe::runtime::ScalarValue>& responses) override;

    std::vector<std::size_t> TerminalPoints(const std::string& run_id) const override;

    std::string ConfigHash(const std::string& run_id) const override;

private:
    void Execute(const std::string& sql);

    sqlite3* db_ = nullptr;
};

}  // namespace doe::persistence

#endif  // DOE_ENGINE_PERSISTENCE_SQLITE_PERSISTENCE_H_
