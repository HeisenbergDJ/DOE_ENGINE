/********************************************************************
 * "sampling.cc" provides the Design Point sampling factory.       *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#include "sampling.h"

#include "full_factorial_sampler.h"

#include <memory>

namespace doe::runtime {

std::unique_ptr<Sampler> MakeSampler(const doe::config::VariablesConfig& variables,
                                     const doe::config::SamplingConfig& sampling)
{
    return std::make_unique<FullFactorialSampler>(variables, sampling);
}

}  // namespace doe::runtime
