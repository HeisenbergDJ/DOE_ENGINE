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
#include "mpi_scheduler.h"
#endif

#include <memory>

namespace doe::runtime {

std::unique_ptr<PointScheduler> MakeScheduler()
{
#ifdef DOE_ENABLE_MPI
    return std::make_unique<MpiScheduler>();
#else
    return std::make_unique<SerialScheduler>();
#endif
}

}  // namespace doe::runtime
