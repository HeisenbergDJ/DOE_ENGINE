/********************************************************************
 * "point_scheduler.cc" provides the point scheduler factory.      *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-16                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#include "point_scheduler.h"

#include "serial_scheduler.h"

#ifdef DOE_ENABLE_MPI
#include <mpi.h>
#include "mpi_scheduler.h"
#endif

#include <memory>

namespace doe::runtime {

bool IsCoordinatorProcess()
{
#ifdef DOE_ENABLE_MPI
    int initialized = 0;
    MPI_Initialized(&initialized);
    if (!initialized) {
        MPI_Init(nullptr, nullptr);
    }
    int rank = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    return rank == 0;
#else
    return true;
#endif
}

std::unique_ptr<PointScheduler> MakeScheduler()
{
#ifdef DOE_ENABLE_MPI
    return std::make_unique<MpiScheduler>();
#else
    return std::make_unique<SerialScheduler>();
#endif
}

}  // namespace doe::runtime
