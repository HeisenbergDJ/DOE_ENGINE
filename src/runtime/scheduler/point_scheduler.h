/********************************************************************
 * "point_scheduler.h" declares the design point scheduler.        *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-16                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#ifndef DOE_ENGINE_RUNTIME_SCHEDULER_POINT_SCHEDULER_H_
#define DOE_ENGINE_RUNTIME_SCHEDULER_POINT_SCHEDULER_H_

#include <cstddef>
#include <functional>
#include <memory>
#include <string>

namespace doe::runtime {

// Aggregate outcome of scheduling one experiment run across (possibly many)
// processes.
struct RunSummary {
    std::size_t point_count = 0;
    std::size_t success_count = 0;
};

// Decides which design point indices each process runs and how the per-process
// results are aggregated. The serial scheduler runs every point locally; the
// MPI scheduler distributes indices over ranks and gathers a global summary.
// run_point(index) returns true when the point succeeded.
class PointScheduler {
public:
    virtual ~PointScheduler() = default;

    // Agrees the run id shared by all processes; the returned value is the one
    // to use for this run.
    virtual std::string BeginRun(const std::string& candidate_run_id) const = 0;

    // True on the coordinating process (rank 0 for MPI, always for serial).
    virtual bool IsRoot() const = 0;

    // Process rank (0 for serial, MPI rank otherwise), used in log tags.
    virtual int Rank() const = 0;

    virtual RunSummary Run(std::size_t point_count,
                           const std::function<bool(std::size_t)>& run_point) const = 0;
};

// Factory for the PointScheduler implementation. Serial is the default; the MPI
// scheduler is selected only when DOE_ENABLE_MPI is defined at build time.
std::unique_ptr<PointScheduler> MakeScheduler();

}  // namespace doe::runtime

#endif  // DOE_ENGINE_RUNTIME_SCHEDULER_POINT_SCHEDULER_H_
