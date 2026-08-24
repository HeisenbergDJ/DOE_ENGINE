/********************************************************************
 * "serial_scheduler.h" declares the serial point scheduler.       *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-16                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#ifndef DOE_ENGINE_RUNTIME_SCHEDULER_SERIAL_SCHEDULER_H_
#define DOE_ENGINE_RUNTIME_SCHEDULER_SERIAL_SCHEDULER_H_

#include "point_scheduler.h"

namespace doe::runtime {

// Runs every design point index locally in order, matching the original serial
// behavior.
class SerialScheduler : public PointScheduler {
public:
    std::string BeginRun(const std::string& candidate_run_id) const override;

    bool IsRoot() const override;

    int Rank() const override;

    RunSummary Run(std::size_t point_count,
                   const std::function<bool(std::size_t)>& run_point) const override;

    ResumeInfo BroadcastResumeInfo(const ResumeInfo& local) const override;

    void SendBatch(const std::vector<std::string>& records) const override;
    void FinishSending() const override;
    void DrainAvailable(
        const std::function<void(const std::vector<std::string>&)>& persist) const override;
    void DrainAll(
        const std::function<void(const std::vector<std::string>&)>& persist) const override;
};

}  // namespace doe::runtime

#endif  // DOE_ENGINE_RUNTIME_SCHEDULER_SERIAL_SCHEDULER_H_
