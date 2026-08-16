/********************************************************************
 * "sampling.h" declares the Design Point sampling interface.      *
 *                                                                  *
 ********************************************************************
 * Author : Yichao Zhang / ChatGPT / Kimi V3                        *
 * E-mail : zhangyichao199@gmail.com                                *
 * Created: 2026-08-15                                              *
 * Copyrights Reserved                                              *
 ********************************************************************/
#ifndef DOE_ENGINE_RUNTIME_SAMPLING_SAMPLING_H_
#define DOE_ENGINE_RUNTIME_SAMPLING_SAMPLING_H_

#include "design_point.h"
#include "sampling_config.h"
#include "variables_config.h"

#include <cstddef>
#include <memory>
#include <vector>

namespace doe::runtime {

// Generates the DesignPoints of one DOE experiment. A Sampler reads no JSON and
// creates no directories. It is constructed with the variables and the sampling
// strategy it belongs to, so the design points are addressable by index and can
// be enumerated lazily without materializing the whole set.
class Sampler {
public:
    virtual ~Sampler() = default;

    // Total number of design points.
    virtual std::size_t Size() const = 0;

    // The index-th design point. Deterministic per index; used to stream design
    // points per rank without materializing the full set.
    virtual DesignPoint At(std::size_t index) const = 0;

    // Materializes every design point in order. Convenience for small designs;
    // prefer Size()/At() for streaming.
    virtual std::vector<DesignPoint> Generate() const = 0;
};

// Factory for Sampler implementations. Concrete sampler types live in their own
// headers, for example full_factorial_sampler.h.
std::unique_ptr<Sampler> MakeSampler(const doe::config::VariablesConfig& variables,
                                     const doe::config::SamplingConfig& sampling);

}  // namespace doe::runtime

#endif  // DOE_ENGINE_RUNTIME_SAMPLING_SAMPLING_H_
