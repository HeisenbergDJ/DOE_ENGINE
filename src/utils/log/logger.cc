/********************************************************************
 * "logger.cc" provides the run log logger.                        *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-16                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#include "logger.h"

#include <chrono>
#include <ctime>
#include <iostream>
#include <string>

namespace doe::log {

namespace {

std::string CurrentTimeIso()
{
    const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    const std::time_t time = std::chrono::system_clock::to_time_t(now);
    const std::chrono::milliseconds milliseconds =
        std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    std::tm local = {};
#ifdef _WIN32
    localtime_s(&local, &time);
#else
    localtime_r(&time, &local);
#endif
    char buffer[32] = {};
    std::strftime(buffer, sizeof buffer, "%Y-%m-%dT%H:%M:%S", &local);
    return std::string(buffer) + "." + std::to_string(milliseconds.count()) + "Z";
}

}  // namespace

Logger::Logger(std::ostream& stream, int rank) : stream_(stream), rank_(rank) {}

void Logger::Info(const std::string& message) const
{
    Write("INFO", message);
}

void Logger::Warn(const std::string& message) const
{
    Write("WARN", message);
}

void Logger::Error(const std::string& message) const
{
    Write("ERROR", message);
}

void Logger::Write(const std::string& level, const std::string& message) const
{
    stream_ << '[' << CurrentTimeIso() << "] [rank " << rank_ << "] " << level << ": " << message
            << std::endl;
}

}  // namespace doe::log
