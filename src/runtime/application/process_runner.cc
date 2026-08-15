/********************************************************************
 * "process_runner.cc" provides the cross-platform process runner. *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#include "process_runner.h"

#include "error.h"

#include <chrono>
#include <string>
#include <utility>
#include <vector>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <cerrno>
#include <cstring>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#endif

namespace doe::runtime {

namespace {

using doe::log::RuntimeError;

#ifdef _WIN32

std::wstring Utf8ToWide(const std::string& text)
{
    if (text.empty()) {
        return {};
    }
    const int size = MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()),
                                         nullptr, 0);
    std::wstring result(static_cast<std::size_t>(size), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), result.data(), size);
    return result;
}

// Quotes an argument following the Win32 command-line rules so arguments with
// spaces and embedded quotes keep their boundaries.
std::wstring QuoteArgument(const std::wstring& argument)
{
    if (argument.empty()) {
        return L"\"\"";
    }
    if (argument.find_first_of(L" \t\"") == std::wstring::npos) {
        return argument;
    }
    std::wstring result = L"\"";
    std::size_t backslashes = 0;
    for (const wchar_t character : argument) {
        if (character == L'\\') {
            ++backslashes;
            continue;
        }
        if (character == L'"') {
            result.append(backslashes * 2 + 1, L'\\');
            result += L'"';
            backslashes = 0;
        } else {
            result.append(backslashes, L'\\');
            result += character;
            backslashes = 0;
        }
    }
    result.append(backslashes * 2, L'\\');
    result += L'"';
    return result;
}

std::wstring BuildCommandLine(const std::wstring& executable,
                              const std::vector<std::string>& arguments)
{
    std::wstring line = QuoteArgument(executable);
    for (const std::string& argument : arguments) {
        line += L' ';
        line += QuoteArgument(Utf8ToWide(argument));
    }
    return line;
}

#endif  // _WIN32

}  // namespace

Process::Process(std::filesystem::path executable, std::filesystem::path working_directory,
                 std::vector<std::string> arguments, long long timeout_seconds)
    : executable_(std::move(executable)),
      working_directory_(std::move(working_directory)),
      arguments_(std::move(arguments)),
      timeout_seconds_(timeout_seconds)
{
}

ProcessResult Process::Run() const
{
    ProcessResult result;

#ifdef _WIN32
    const std::wstring command_line = BuildCommandLine(executable_.wstring(), arguments_);
    const std::wstring working_directory = working_directory_.wstring();
    std::vector<wchar_t> command_buffer(command_line.begin(), command_line.end());
    command_buffer.push_back(L'\0');

    STARTUPINFOW startup_info{};
    startup_info.cb = sizeof(startup_info);
    PROCESS_INFORMATION process_info{};
    const BOOL created = CreateProcessW(
        nullptr, command_buffer.data(), nullptr, nullptr, FALSE, 0, nullptr,
        working_directory.empty() ? nullptr : working_directory.c_str(), &startup_info,
        &process_info);
    if (!created) {
        throw RuntimeError("process", executable_.string(), working_directory_,
                           "failed to launch process (error " + std::to_string(GetLastError()) + ")");
    }
    CloseHandle(process_info.hThread);

    const DWORD timeout_ms =
        timeout_seconds_ > 0 ? static_cast<DWORD>(timeout_seconds_ * 1000) : INFINITE;
    const DWORD wait = WaitForSingleObject(process_info.hProcess, timeout_ms);
    if (wait == WAIT_TIMEOUT) {
        TerminateProcess(process_info.hProcess, 1);
        WaitForSingleObject(process_info.hProcess, INFINITE);
        result.timed_out = true;
        result.exit_code = -1;
    } else {
        DWORD exit_code = 0;
        GetExitCodeProcess(process_info.hProcess, &exit_code);
        result.exit_code = static_cast<int>(exit_code);
    }
    CloseHandle(process_info.hProcess);
    return result;

#else
    const pid_t pid = fork();
    if (pid < 0) {
        throw RuntimeError("process", executable_.string(), working_directory_,
                           "failed to fork the process");
    }
    if (pid == 0) {
        if (!working_directory_.empty() && chdir(working_directory_.c_str()) != 0) {
            _exit(127);
        }
        std::vector<char*> argv;
        argv.reserve(arguments_.size() + 2);
        argv.push_back(const_cast<char*>(executable_.c_str()));
        for (const std::string& argument : arguments_) {
            argv.push_back(const_cast<char*>(argument.c_str()));
        }
        argv.push_back(nullptr);
        execvp(executable_.c_str(), argv.data());
        _exit(127);
    }

    int status = 0;
    if (timeout_seconds_ <= 0) {
        if (waitpid(pid, &status, 0) < 0) {
            throw RuntimeError("process", executable_.string(), working_directory_,
                               std::string("failed to wait for the process: ") + std::strerror(errno));
        }
    } else {
        const auto deadline =
            std::chrono::steady_clock::now() + std::chrono::seconds(timeout_seconds_);
        for (;;) {
            const pid_t waited = waitpid(pid, &status, WNOHANG);
            if (waited == pid) {
                break;
            }
            if (waited < 0) {
                if (errno == EINTR) {
                    continue;
                }
                throw RuntimeError("process", executable_.string(), working_directory_,
                                   std::string("failed to wait for the process: ") +
                                       std::strerror(errno));
            }
            if (std::chrono::steady_clock::now() >= deadline) {
                kill(pid, SIGKILL);
                waitpid(pid, &status, 0);
                result.timed_out = true;
                result.exit_code = -1;
                return result;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    if (WIFEXITED(status)) {
        result.exit_code = WEXITSTATUS(status);
    } else {
        result.exit_code = -1;
    }
    return result;
#endif
}

}  // namespace doe::runtime
