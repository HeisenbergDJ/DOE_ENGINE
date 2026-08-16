/********************************************************************
 * "logger.h" declares the run log logger.                         *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-16                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#ifndef DOE_ENGINE_UTILS_LOG_LOGGER_H_
#define DOE_ENGINE_UTILS_LOG_LOGGER_H_

#include <iosfwd>
#include <string>

namespace doe::log {

// Writes timestamped, rank-tagged log lines to a stream. Used to give a
// multi-process MPI run an auditable per-run log with progress and errors; each
// line is flushed immediately so a tail -f sees progress in near real time.
class Logger {
public:
    Logger(std::ostream& stream, int rank);

    void Info(const std::string& message) const;
    void Warn(const std::string& message) const;
    void Error(const std::string& message) const;

private:
    void Write(const std::string& level, const std::string& message) const;

    std::ostream& stream_;
    int rank_;
};

}  // namespace doe::log

#endif  // DOE_ENGINE_UTILS_LOG_LOGGER_H_
