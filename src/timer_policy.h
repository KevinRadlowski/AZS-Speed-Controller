#pragma once
#include <cstdint>

namespace azs {
constexpr std::uintptr_t timerSleepReturnRva = 0x05b963c8;
constexpr std::uint32_t simulationSleepNs = 25000000;

// Only the identified local simulation call may change; other Rust sleeps pass through.
constexpr std::uint32_t timerSleepNanoseconds(std::uintptr_t callerRva,
    std::uint64_t seconds, std::uint32_t nanos, long speed100) {
  if(callerRva != timerSleepReturnRva || seconds != 0 ||
      nanos != simulationSleepNs || speed100 < 100 || speed100 > 2500) return nanos;
  return static_cast<std::uint32_t>((std::uint64_t(nanos) * 100) / speed100);
}
}
