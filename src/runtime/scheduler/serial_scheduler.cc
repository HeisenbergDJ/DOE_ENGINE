/********************************************************************
 * "serial_scheduler.cc" provides the serial point scheduler.      *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-16                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#include "serial_scheduler.h"

#include <functional>

namespace doe::runtime {

std::string SerialScheduler::BeginRun(const std::string& candidate_run_id) const
{
    return candidate_run_id;
}

bool SerialScheduler::IsRoot() const
{
    return true;
}

int SerialScheduler::Rank() const
{
    return 0;
}

RunSummary SerialScheduler::Run(std::size_t point_count,
                                const std::function<bool(std::size_t)>& run_point) const
{
    RunSummary summary;
    summary.point_count = point_count;
    for (std::size_t index = 0; index < point_count; ++index) {
        if (run_point(index)) {
            ++summary.success_count;
        }
    }
    return summary;
}

}  // namespace doe::runtime
