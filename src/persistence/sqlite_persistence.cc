/********************************************************************
 * "sqlite_persistence.cc" provides the SQLite persistence impl.   *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-16                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#include "sqlite_persistence.h"

#include "error.h"
#include "json.hpp"

#include <sqlite3.h>

#include <sstream>
#include <utility>
#include <variant>
#include <vector>

namespace doe::persistence {

namespace {

using doe::log::RuntimeError;

nlohmann::json ScalarToJson(const doe::runtime::ScalarValue& value)
{
    return std::visit([](const auto& alternative) -> nlohmann::json { return alternative; },
                      value);
}

std::string ScalarMapToJson(const std::map<std::string, doe::runtime::ScalarValue>& values)
{
    nlohmann::json object = nlohmann::json::object();
    for (const auto& entry : values) {
        object[entry.first] = ScalarToJson(entry.second);
    }
    return object.dump();
}

std::string OutputsToJson(const std::map<std::string, std::filesystem::path>& outputs)
{
    nlohmann::json object = nlohmann::json::object();
    for (const auto& entry : outputs) {
        object[entry.first] = entry.second.string();
    }
    return object.dump();
}

// RAII wrapper around a prepared SQLite statement.
class Statement {
public:
    Statement(sqlite3* db, const std::string& sql, const std::string& object_id)
        : db_(db), object_id_(object_id)
    {
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt_, nullptr) != SQLITE_OK) {
            throw RuntimeError("persistence", object_id_, std::filesystem::path(),
                               std::string("failed to prepare statement: ") +
                                   sqlite3_errmsg(db_));
        }
    }

    ~Statement()
    {
        sqlite3_finalize(stmt_);
    }

    sqlite3_stmt* get() const
    {
        return stmt_;
    }

    void Step()
    {
        if (sqlite3_step(stmt_) != SQLITE_DONE) {
            throw RuntimeError("persistence", object_id_, std::filesystem::path(),
                               std::string("failed to execute statement: ") +
                                   sqlite3_errmsg(db_));
        }
    }

private:
    sqlite3* db_;
    std::string object_id_;
    sqlite3_stmt* stmt_ = nullptr;
};

}  // namespace

SqlitePersistence::SqlitePersistence(const std::filesystem::path& db_path)
{
    if (sqlite3_open_v2(db_path.string().c_str(), &db_,
                        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr) != SQLITE_OK) {
        std::string message = db_ ? sqlite3_errmsg(db_) : "failed to open the database";
        if (db_ != nullptr) {
            sqlite3_close(db_);
            db_ = nullptr;
        }
        throw RuntimeError("persistence", "", db_path, message);
    }
    sqlite3_busy_timeout(db_, 10000);
    Execute("PRAGMA journal_mode=WAL;");
    Execute("CREATE TABLE IF NOT EXISTS runs ("
            "run_id TEXT PRIMARY KEY,"
            "config_hash TEXT NOT NULL,"
            "status TEXT NOT NULL,"
            "started_at TEXT,"
            "finished_at TEXT);");
    Execute("CREATE TABLE IF NOT EXISTS design_points ("
            "run_id TEXT NOT NULL,"
            "point_index INTEGER NOT NULL,"
            "status TEXT NOT NULL,"
            "variables_json TEXT,"
            "responses_json TEXT,"
            "error_message TEXT,"
            "finished_at TEXT,"
            "PRIMARY KEY (run_id, point_index));");
    Execute("CREATE TABLE IF NOT EXISTS application_runs ("
            "run_id TEXT NOT NULL,"
            "point_index INTEGER NOT NULL,"
            "application_id TEXT NOT NULL,"
            "outputs_json TEXT,"
            "recorded_at TEXT,"
            "PRIMARY KEY (run_id, point_index, application_id));");
}

SqlitePersistence::~SqlitePersistence()
{
    if (db_ != nullptr) {
        sqlite3_close(db_);
    }
}

void SqlitePersistence::Execute(const std::string& sql)
{
    char* error = nullptr;
    if (sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &error) != SQLITE_OK) {
        std::string message = error ? error : "sqlite error";
        sqlite3_free(error);
        throw RuntimeError("persistence", "", std::filesystem::path(), message);
    }
}

void SqlitePersistence::BeginBatch()
{
    Execute("BEGIN;");
}

void SqlitePersistence::EndBatch()
{
    Execute("COMMIT;");
}

void SqlitePersistence::AbortBatch()
{
    Execute("ROLLBACK;");
}

void SqlitePersistence::RecordExperimentStart(const std::string& run_id,
                                              const std::string& config_hash)
{
    // INSERT OR REPLACE lets a resumed run reopen its existing run row.
    Statement statement(
        db_, "INSERT OR REPLACE INTO runs (run_id, config_hash, status, started_at) "
             "VALUES (?, ?, 'RUNNING', strftime('%Y-%m-%dT%H:%M:%fZ','now'))",
        run_id);
    sqlite3_bind_text(statement.get(), 1, run_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement.get(), 2, config_hash.c_str(), -1, SQLITE_TRANSIENT);
    statement.Step();
}

void SqlitePersistence::RecordExperimentEnd(const std::string& run_id, bool success)
{
    Statement statement(
        db_, "UPDATE runs SET status=?, finished_at=strftime('%Y-%m-%dT%H:%M:%fZ','now') "
             "WHERE run_id=?",
        run_id);
    sqlite3_bind_text(statement.get(), 1, success ? "SUCCESS" : "FAILED", -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement.get(), 2, run_id.c_str(), -1, SQLITE_TRANSIENT);
    statement.Step();
}

void SqlitePersistence::RecordApplicationRun(
    const std::string& run_id, std::size_t point_index, const std::string& application_id,
    const std::map<std::string, std::filesystem::path>& outputs)
{
    Statement statement(
        db_, "INSERT INTO application_runs (run_id, point_index, application_id, outputs_json, "
             "recorded_at) VALUES (?, ?, ?, ?, strftime('%Y-%m-%dT%H:%M:%fZ','now'))",
        run_id);
    sqlite3_bind_text(statement.get(), 1, run_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(statement.get(), 2, static_cast<sqlite3_int64>(point_index));
    sqlite3_bind_text(statement.get(), 3, application_id.c_str(), -1, SQLITE_TRANSIENT);
    const std::string outputs_json = OutputsToJson(outputs);
    sqlite3_bind_text(statement.get(), 4, outputs_json.c_str(), -1, SQLITE_TRANSIENT);
    statement.Step();
}

void SqlitePersistence::RecordPointOutcome(
    const std::string& run_id, std::size_t point_index,
    const std::map<std::string, doe::runtime::ScalarValue>& variables, bool success,
    const std::string& error_message,
    const std::map<std::string, doe::runtime::ScalarValue>& responses)
{
    // INSERT OR REPLACE lets a resumed run rewrite the row of a point it
    // re-ran, instead of failing on the primary key.
    Statement statement(
        db_, "INSERT OR REPLACE INTO design_points (run_id, point_index, status, variables_json, "
             "responses_json, error_message, finished_at) "
             "VALUES (?, ?, ?, ?, ?, ?, strftime('%Y-%m-%dT%H:%M:%fZ','now'))",
        run_id);
    sqlite3_bind_text(statement.get(), 1, run_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(statement.get(), 2, static_cast<sqlite3_int64>(point_index));
    sqlite3_bind_text(statement.get(), 3, success ? "SUCCESS" : "FAILED", -1, SQLITE_TRANSIENT);
    const std::string variables_json = ScalarMapToJson(variables);
    sqlite3_bind_text(statement.get(), 4, variables_json.c_str(), -1, SQLITE_TRANSIENT);
    const std::string responses_json = ScalarMapToJson(responses);
    sqlite3_bind_text(statement.get(), 5, responses_json.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement.get(), 6, error_message.c_str(), -1, SQLITE_TRANSIENT);
    statement.Step();
}

std::vector<std::size_t> SqlitePersistence::TerminalPoints(const std::string& run_id) const
{
    std::vector<std::size_t> points;
    sqlite3_stmt* statement = nullptr;
    if (sqlite3_prepare_v2(
            db_, "SELECT point_index FROM design_points WHERE run_id=? AND "
                 "status IN ('SUCCESS','FAILED')",
            -1, &statement, nullptr) != SQLITE_OK) {
        throw RuntimeError("persistence", run_id, std::filesystem::path(),
                           std::string("failed to prepare statement: ") + sqlite3_errmsg(db_));
    }
    sqlite3_bind_text(statement, 1, run_id.c_str(), -1, SQLITE_TRANSIENT);
    int result = SQLITE_OK;
    while ((result = sqlite3_step(statement)) == SQLITE_ROW) {
        points.push_back(static_cast<std::size_t>(sqlite3_column_int64(statement, 0)));
    }
    sqlite3_finalize(statement);
    if (result != SQLITE_DONE) {
        throw RuntimeError("persistence", run_id, std::filesystem::path(),
                           std::string("failed to query terminal points: ") +
                               sqlite3_errmsg(db_));
    }
    return points;
}

std::string SqlitePersistence::ConfigHash(const std::string& run_id) const
{
    sqlite3_stmt* statement = nullptr;
    if (sqlite3_prepare_v2(db_, "SELECT config_hash FROM runs WHERE run_id=?", -1, &statement,
                           nullptr) != SQLITE_OK) {
        throw RuntimeError("persistence", run_id, std::filesystem::path(),
                           std::string("failed to prepare statement: ") + sqlite3_errmsg(db_));
    }
    sqlite3_bind_text(statement, 1, run_id.c_str(), -1, SQLITE_TRANSIENT);
    std::string config_hash;
    const int result = sqlite3_step(statement);
    if (result == SQLITE_ROW && sqlite3_column_text(statement, 0) != nullptr) {
        config_hash = reinterpret_cast<const char*>(sqlite3_column_text(statement, 0));
    }
    sqlite3_finalize(statement);
    if (result != SQLITE_ROW && result != SQLITE_DONE) {
        throw RuntimeError("persistence", run_id, std::filesystem::path(),
                           std::string("failed to query config hash: ") + sqlite3_errmsg(db_));
    }
    return config_hash;
}

}  // namespace doe::persistence
