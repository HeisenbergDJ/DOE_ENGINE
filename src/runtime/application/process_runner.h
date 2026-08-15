/********************************************************************
 * "process_runner.h" declares the cross-platform process runner.  *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#ifndef DOE_ENGINE_RUNTIME_APPLICATION_PROCESS_RUNNER_H_
#define DOE_ENGINE_RUNTIME_APPLICATION_PROCESS_RUNNER_H_

#include <filesystem>
#include <string>
#include <vector>

namespace doe::runtime {

struct ProcessResult {
    int exit_code = -1;
    bool timed_out = false;
};

// Launches one external program with an explicit argument list and waits for
// it. Windows and POSIX implementations are kept behind this one interface;
// no shell string is built and argument boundaries are preserved.
class Process {
public:
    Process(std::filesystem::path executable,
            std::filesystem::path working_directory,
            std::vector<std::string> arguments,
            long long timeout_seconds);

    ProcessResult Run() const;

private:
    std::filesystem::path executable_;
    std::filesystem::path working_directory_;
    std::vector<std::string> arguments_;
    long long timeout_seconds_;
};

}  // namespace doe::runtime

#endif  // DOE_ENGINE_RUNTIME_APPLICATION_PROCESS_RUNNER_H_
