/********************************************************************
 * "mpi_scheduler.cc" provides the MPI point scheduler.           *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-16                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#include "mpi_scheduler.h"

#include <mpi.h>

#include <algorithm>
#include <cstring>
#include <string>

namespace doe::runtime {

namespace {

constexpr int kRunIdBufferSize = 64;

}  // namespace

MpiScheduler::MpiScheduler()
{
    int initialized = 0;
    MPI_Initialized(&initialized);
    if (!initialized) {
        MPI_Init(nullptr, nullptr);
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_);
    MPI_Comm_size(MPI_COMM_WORLD, &size_);
}

MpiScheduler::~MpiScheduler()
{
    int finalized = 0;
    MPI_Finalized(&finalized);
    if (!finalized) {
        MPI_Finalize();
    }
}

std::string MpiScheduler::BeginRun(const std::string& candidate_run_id) const
{
    char buffer[kRunIdBufferSize] = {};
    if (rank_ == 0) {
        std::strncpy(buffer, candidate_run_id.c_str(), kRunIdBufferSize - 1);
    }
    MPI_Bcast(buffer, kRunIdBufferSize, MPI_CHAR, 0, MPI_COMM_WORLD);
    return std::string(buffer);
}

bool MpiScheduler::IsRoot() const
{
    return rank_ == 0;
}

int MpiScheduler::Rank() const
{
    return rank_;
}

RunSummary MpiScheduler::Run(std::size_t point_count,
                             const std::function<bool(std::size_t)>& run_point) const
{
    const std::size_t block =
        (point_count + static_cast<std::size_t>(size_) - 1) / static_cast<std::size_t>(size_);
    const std::size_t begin = static_cast<std::size_t>(rank_) * block;
    const std::size_t end = std::min(begin + block, point_count);

    unsigned long long local_success = 0;
    for (std::size_t index = begin; index < end; ++index) {
        if (run_point(index)) {
            ++local_success;
        }
    }

    unsigned long long global_success = 0;
    MPI_Allreduce(&local_success, &global_success, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM,
                  MPI_COMM_WORLD);

    RunSummary summary;
    summary.point_count = point_count;
    summary.success_count = static_cast<std::size_t>(global_success);
    return summary;
}

}  // namespace doe::runtime
