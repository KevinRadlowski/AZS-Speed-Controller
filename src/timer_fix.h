#pragma once
#include <windows.h>
// Creates an optional MinHook hook; caller enables it with the clock hooks.
// Shared active status: 1 = clocks only, 2 = chrono armed, 3 = matching call observed.
bool prepareTimerFix(LONG (*readSpeed)(), volatile LONG* active);
