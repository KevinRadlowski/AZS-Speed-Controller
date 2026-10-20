#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <MinHook.h>
#include <mutex>
#include <cstdint>

struct Shared { volatile LONG speed100; volatile LONG active; };
static Shared* shared = nullptr;
static HANDLE mapping = nullptr;
static std::mutex timeLock;
using QpcFn = BOOL (WINAPI*)(LARGE_INTEGER*);
using TickFn = DWORD (WINAPI*)();
using Tick64Fn = ULONGLONG (WINAPI*)();
using TimeFn = DWORD (WINAPI*)();
static QpcFn originalQpc = nullptr;
static TickFn originalTick = nullptr;
static Tick64Fn originalTick64 = nullptr;
static TimeFn originalTime = nullptr;
struct Clock { uint64_t real = 0, virtualTime = 0; LONG speed = 100; bool initialized = false; };
static Clock qpcClock, tickClock, tick64Clock, timeClock;

static uint64_t scaleClock(Clock& c, uint64_t now) {
  std::lock_guard<std::mutex> guard(timeLock);
  LONG next = shared ? InterlockedCompareExchange(&shared->speed100, 0, 0) : 100;
  if(next < 100 || next > 2500) next = 100;
  if(!c.initialized) { c.real=now; c.virtualTime=now; c.speed=next; c.initialized=true; return now; }
  uint64_t delta = now >= c.real ? now-c.real : 0;
  c.virtualTime += (delta * static_cast<uint64_t>(c.speed)) / 100;
  c.real=now; c.speed=next;
  return c.virtualTime;
}
static BOOL WINAPI hookedQpc(LARGE_INTEGER* out) {
  LARGE_INTEGER actual{};
  if(!originalQpc(&actual)) return FALSE;
  out->QuadPart=static_cast<LONGLONG>(scaleClock(qpcClock,static_cast<uint64_t>(actual.QuadPart)));
  return TRUE;
}
static DWORD WINAPI hookedTick() { return static_cast<DWORD>(scaleClock(tickClock,originalTick())); }
static ULONGLONG WINAPI hookedTick64() { return scaleClock(tick64Clock,originalTick64()); }
static DWORD WINAPI hookedTime() { return static_cast<DWORD>(scaleClock(timeClock,originalTime())); }

// Create all hooks before enabling any. On any failure, remove every hook
// created during this initialization attempt so no partial speed modification
// is left behind. MH_ALL_HOOKS enables the batch in one MinHook operation.
static bool createHook(HMODULE module, const char* name, LPVOID replacement, LPVOID* original) {
  if(!module) return false;
  FARPROC address = GetProcAddress(module, name);
  return address && MH_CreateHook(reinterpret_cast<LPVOID>(address), replacement, original)==MH_OK;
}

static DWORD WINAPI init(LPVOID) {
  wchar_t key[96]; wsprintfW(key,L"Local\\AZSSpeed_%lu",GetCurrentProcessId());
  HANDLE localMapping=CreateFileMappingW(INVALID_HANDLE_VALUE,nullptr,PAGE_READWRITE,0,sizeof(Shared),key);
  if(!localMapping) return 0;
  DWORD mappingError=GetLastError();
  Shared* localShared=static_cast<Shared*>(MapViewOfFile(localMapping,FILE_MAP_ALL_ACCESS,0,0,sizeof(Shared)));
  if(!localShared) { CloseHandle(localMapping); return 0; }
  mapping=localMapping;
  shared=localShared;
  if(mappingError!=ERROR_ALREADY_EXISTS) InterlockedExchange(&shared->speed100,100);
  InterlockedExchange(&shared->active,0);

  if(MH_Initialize()!=MH_OK) {
    UnmapViewOfFile(localShared); shared=nullptr;
    CloseHandle(localMapping); mapping=nullptr;
    return 0;
  }

  HMODULE kernel=GetModuleHandleW(L"kernel32.dll");
  HMODULE winmm=LoadLibraryW(L"winmm.dll");
  const bool created =
    createHook(kernel,"QueryPerformanceCounter",reinterpret_cast<LPVOID>(hookedQpc),reinterpret_cast<LPVOID*>(&originalQpc)) &&
    createHook(kernel,"GetTickCount",reinterpret_cast<LPVOID>(hookedTick),reinterpret_cast<LPVOID*>(&originalTick)) &&
    createHook(kernel,"GetTickCount64",reinterpret_cast<LPVOID>(hookedTick64),reinterpret_cast<LPVOID*>(&originalTick64)) &&
    createHook(winmm,"timeGetTime",reinterpret_cast<LPVOID>(hookedTime),reinterpret_cast<LPVOID*>(&originalTime));
  if(!created || MH_EnableHook(MH_ALL_HOOKS)!=MH_OK) {
    // No active hook should remain after a failed initialization.
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
    if(winmm) FreeLibrary(winmm);
    UnmapViewOfFile(localShared); shared=nullptr;
    CloseHandle(localMapping); mapping=nullptr;
    return 0;
  }
  // Controller connects only after all expected hooks have been enabled.
  InterlockedExchange(&shared->active,1);
  // Keep mapping and winmm loaded while hooks are active. Windows reclaims
  // them at process exit; unhooking in DllMain during teardown is unsafe.
  return 0;
}
BOOL APIENTRY DllMain(HMODULE h,DWORD reason,LPVOID) {
  if(reason==DLL_PROCESS_ATTACH) {
    DisableThreadLibraryCalls(h);
    HANDLE t=CreateThread(nullptr,0,init,nullptr,0,nullptr);
    if(t) CloseHandle(t);
  }
  return TRUE;
}
