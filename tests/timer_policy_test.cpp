#include "../src/timer_policy.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>
#include <cmath>
#include <iostream>

int main() {
  using namespace azs;
  for(long speed : {100L,200L,500L,1000L,1500L,2500L}) {
    auto delay = timerSleepNanoseconds(timerSleepReturnRva,0,simulationSleepNs,speed);
    assert(delay >= 1000000 && delay <= simulationSleepNs);
    double factor = speed / 100.0;
    double oldRate = std::min(0.025*factor,0.1) / 0.025;
    double physicalStep = delay / 1e9;
    double newRate = std::min(physicalStep*factor,0.1) / physicalStep;
    assert(std::abs(newRate-factor)<1e-8);
    if(speed >= 500) assert(std::abs(oldRate-4.0)<1e-8);
    // A different caller, seconds component or different wait must pass through.
    assert(timerSleepNanoseconds(timerSleepReturnRva+1,0,simulationSleepNs,speed)==simulationSleepNs);
    assert(timerSleepNanoseconds(timerSleepReturnRva,1,simulationSleepNs,speed)==simulationSleepNs);
    assert(timerSleepNanoseconds(timerSleepReturnRva,0,1000000,speed)==1000000);
  }
  for(long speed : {-1L,0L,99L,2501L,100000L})
    assert(timerSleepNanoseconds(timerSleepReturnRva,0,simulationSleepNs,speed)==simulationSleepNs);
  std::cout << "Timer targeting, bounds and idealized simulation rates: PASS\n";
}
