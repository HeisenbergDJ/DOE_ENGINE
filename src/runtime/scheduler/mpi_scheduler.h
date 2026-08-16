/********************************************************************
 * "mpi_scheduler.h" declares the MPI point scheduler.            *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-16                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#ifndef DOE_ENGINE_RUNTIME_SCHEDULER_MPI_SCHEDULER_H_
#define DOE_ENGINE_RUNTIME_SCHEDULER_MPI_SCHEDULER_H_

#include "point_scheduler.h"

namespace doe::runtime {

// Distributes design point indices over MPI ranks as contiguous blocks and
// aggregates a global success count on every rank. The constructor initializes
// MPI if needed; the destructor finalizes it.
class MpiScheduler : public PointScheduler {
public:
    MpiScheduler();
    ~MpiScheduler() override;

    std::string BeginRun(const std::string& candidate_run_id) const override;

    bool IsRoot() const override;

    int Rank() const override;

    RunSummary Run(std::size_t point_count,
                   const std::function<bool(std::size_t)>& run_point) const override;

private:
    int rank_ = 0;
    int size_ = 1;
};

}  // namespace doe::runtime

#endif  // DOE_ENGINE_RUNTIME_SCHEDULER_MPI_SCHEDULER_H_
