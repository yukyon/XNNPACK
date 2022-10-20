// Copyright 2022 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <algorithm>
#include <cmath>
#include <functional>
#include <random>
#include <vector>

#include "bench/utils.h"
#include <benchmark/benchmark.h>

#include <xnnpack.h>
#include <xnnpack/aligned-allocator.h>
#include <xnnpack/common.h>
#include <xnnpack/microfnptr.h>
#include <xnnpack/vlshift.h>


void i16_vlshift(
    benchmark::State& state,
    xnn_i16_vlshift_ukernel_function vlshift,
    benchmark::utils::IsaCheckFunction isa_check = nullptr)
{
  if (isa_check && !isa_check(state)) {
    return;
  }
  const size_t batch = state.range(0);

  std::random_device random_device;
  auto rng = std::mt19937(random_device());
  auto u16rng = std::bind(std::uniform_int_distribution<uint16_t>(), std::ref(rng));

  std::vector<uint16_t, AlignedAllocator<uint16_t, 64>> input(batch + XNN_EXTRA_BYTES / sizeof(uint16_t));
  std::vector<uint16_t, AlignedAllocator<uint16_t, 64>> output(batch);

  std::generate(input.begin(), input.end(), std::ref(u16rng));
  std::fill(output.begin(), output.end(), UINT16_C(0xDEAD));

  for (auto _ : state) {
    vlshift(batch, input.data(), output.data(), 4 /* shift */);
  }

  const uint64_t cpu_frequency = benchmark::utils::GetCurrentCpuFrequency();
  if (cpu_frequency != 0) {
    state.counters["cpufreq"] = cpu_frequency;
  }
}

#if XNN_ARCH_ARM || XNN_ARCH_ARM64
  BENCHMARK_CAPTURE(i16_vlshift, i16_neon_x8,
                    xnn_i16_vlshift_ukernel__neon_x8,
                    benchmark::utils::CheckNEON)
    ->Apply(benchmark::utils::UnaryElementwiseParameters<uint16_t, uint16_t>)
    ->UseRealTime();
  BENCHMARK_CAPTURE(i16_vlshift, i16_neon_x16,
                    xnn_i16_vlshift_ukernel__neon_x16,
                    benchmark::utils::CheckNEON)
    ->Apply(benchmark::utils::UnaryElementwiseParameters<uint16_t, uint16_t>)
    ->UseRealTime();
  BENCHMARK_CAPTURE(i16_vlshift, i16_neon_x24,
                    xnn_i16_vlshift_ukernel__neon_x24,
                    benchmark::utils::CheckNEON)
    ->Apply(benchmark::utils::UnaryElementwiseParameters<uint16_t, uint16_t>)
    ->UseRealTime();
  BENCHMARK_CAPTURE(i16_vlshift, i16_neon_x32,
                    xnn_i16_vlshift_ukernel__neon_x32,
                    benchmark::utils::CheckNEON)
    ->Apply(benchmark::utils::UnaryElementwiseParameters<uint16_t, uint16_t>)
    ->UseRealTime();
#endif  // XNN_ARCH_ARM || XNN_ARCH_ARM64

BENCHMARK_CAPTURE(i16_vlshift, i16_scalar_x1,
                  xnn_i16_vlshift_ukernel__scalar_x1)
    ->Apply(benchmark::utils::UnaryElementwiseParameters<uint16_t, uint16_t>)
    ->UseRealTime();
BENCHMARK_CAPTURE(i16_vlshift, i16_scalar_x2,
                  xnn_i16_vlshift_ukernel__scalar_x2)
    ->Apply(benchmark::utils::UnaryElementwiseParameters<uint16_t, uint16_t>)
    ->UseRealTime();
BENCHMARK_CAPTURE(i16_vlshift, i16_scalar_x3,
                  xnn_i16_vlshift_ukernel__scalar_x3)
    ->Apply(benchmark::utils::UnaryElementwiseParameters<uint16_t, uint16_t>)
    ->UseRealTime();
BENCHMARK_CAPTURE(i16_vlshift, i16_scalar_x4,
                  xnn_i16_vlshift_ukernel__scalar_x4)
    ->Apply(benchmark::utils::UnaryElementwiseParameters<uint16_t, uint16_t>)
    ->UseRealTime();

#ifndef XNNPACK_BENCHMARK_NO_MAIN
BENCHMARK_MAIN();
#endif
