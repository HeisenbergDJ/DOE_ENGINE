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
#include <vector>

namespace doe::runtime {

// Aggregate outcome of scheduling one experiment run across (possibly many)
// processes.
struct RunSummary {
    std::size_t point_count = 0;
    std::size_t success_count = 0;
};

// The result of validating a resume request on the root process. Under MPI the
// root broadcasts this to every rank so all ranks either agree to skip the
// already-completed design points or abort together.
struct ResumeInfo {
    bool abort = false;
    std::string abort_message;
    std::vector<std::size_t> terminal_points;
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

    // Coordinates the resume decision: the root's ResumeInfo is shared with
    // every rank and returned on all of them.
    virtual ResumeInfo BroadcastResumeInfo(const ResumeInfo& local) const = 0;

    // Streaming persistence channel. While the run is still computing, every
    // non-root rank ships the finished design point records it produces to the
    // root, and the root persists them as they arrive, so database writes
    // overlap with computation and the root never holds the whole run in
    // memory. Each of these is a collective-free point-to-point operation.
    //
    // SendBatch() ships one batch of records from a non-root rank to the root.
    // FinishSending() is called once after the rank has sent its last batch;
    // it tells the root there is nothing more coming from that rank.
    virtual void SendBatch(const std::vector<std::string>& records) const = 0;
    virtual void FinishSending() const = 0;

    // Root-only helpers. DrainAvailable() persists every batch already in
    // flight and returns without blocking; DrainAll() blocks until every
    // non-root rank has finished and persists the remaining batches.
    virtual void DrainAvailable(
        const std::function<void(const std::vector<std::string>&)>& persist) const = 0;
    virtual void DrainAll(
        const std::function<void(const std::vector<std::string>&)>& persist) const = 0;
};

// True on the process that may open the persistence database: the MPI root
// (rank 0), or always for a serial build.
bool IsCoordinatorProcess();

// Factory for the PointScheduler implementation. Serial is the default; the MPI
// scheduler is selected only when DOE_ENABLE_MPI is defined at build time.
std::unique_ptr<PointScheduler> MakeScheduler();

}  // namespace doe::runtime

#endif  // DOE_ENGINE_RUNTIME_SCHEDULER_POINT_SCHEDULER_H_
