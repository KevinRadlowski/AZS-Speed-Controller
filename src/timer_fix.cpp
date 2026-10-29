#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <bcrypt.h>
#include <MinHook.h>
#include <array>
#include <cstring>
#include <vector>
#ifdef _MSC_VER
#include <intrin.h>
#endif
#include "timer_fix.h"
#include "timer_policy.h"

namespace {
using RustSleepFn = void (*)(std::uint64_t, std::uint32_t);
RustSleepFn originalSleep = nullptr;
LONG (*getSpeed)() = nullptr;
volatile LONG* activeStatus = nullptr;
std::uintptr_t imageBase = 0;

bool exactFileHash() {
  // Full hash of the uploaded 0.127.0; version text alone does not identify a build.
  constexpr unsigned char expected[] = {
    0x3e,0xf9,0xd7,0x4c,0x30,0xbb,0x6b,0x43,0x5d,0x73,0x9b,0x3d,0x96,0x0c,0x85,0x35,
    0x48,0x68,0x36,0xc5,0x40,0xe0,0xd8,0xc2,0x0f,0x98,0x54,0x9a,0x6f,0x1d,0xcb,0xbb
  };
  wchar_t path[32768]{};
  DWORD length = GetModuleFileNameW(nullptr,path,32768);
  if(!length || length >= 32768) return false;
  HANDLE file = CreateFileW(path,GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,
      FILE_FLAG_SEQUENTIAL_SCAN,nullptr);
  if(file == INVALID_HANDLE_VALUE) return false;
  BCRYPT_ALG_HANDLE alg = nullptr;
  BCRYPT_HASH_HANDLE hash = nullptr;
  DWORD objectSize = 0, received = 0;
  bool ok = BCryptOpenAlgorithmProvider(&alg,BCRYPT_SHA256_ALGORITHM,nullptr,0) >= 0;
  if(ok) ok = BCryptGetProperty(alg,BCRYPT_OBJECT_LENGTH,
      reinterpret_cast<PUCHAR>(&objectSize),sizeof(objectSize),&received,0) >= 0;
  std::vector<unsigned char> object(ok ? objectSize : 0);
  if(ok) ok = BCryptCreateHash(alg,&hash,object.data(),objectSize,nullptr,0,0) >= 0;
  std::array<unsigned char,65536> buffer{};
  while(ok) {
    DWORD count = 0;
    if(!ReadFile(file,buffer.data(),static_cast<DWORD>(buffer.size()),&count,nullptr)) { ok=false; break; }
    if(!count) break;
    ok = BCryptHashData(hash,buffer.data(),count,0) >= 0;
  }
  unsigned char digest[32]{};
  if(ok) ok = BCryptFinishHash(hash,digest,sizeof(digest),0) >= 0 &&
      std::memcmp(digest,expected,sizeof(expected)) == 0;
  if(hash) BCryptDestroyHash(hash);
  if(alg) BCryptCloseAlgorithmProvider(alg,0);
  CloseHandle(file);
  return ok;
}

template<std::size_t N> bool matches(const unsigned char* base,
    std::size_t rva, const unsigned char (&expected)[N], std::size_t imageSize) {
  if(rva > imageSize || N > imageSize-rva) return false;
  MEMORY_BASIC_INFORMATION info{};
  if(!VirtualQuery(base+rva,&info,sizeof(info)) || info.State != MEM_COMMIT ||
      (info.Protect & (PAGE_NOACCESS|PAGE_GUARD))) return false;
  const auto remaining = static_cast<const unsigned char*>(info.BaseAddress) + info.RegionSize - (base+rva);
  return remaining >= static_cast<std::ptrdiff_t>(N) && std::memcmp(base+rva,expected,N)==0;
}

// Must capture the game's return address here, before any helper is called.
#ifdef _MSC_VER
__declspec(noinline)
#else
__attribute__((noinline))
#endif
void hookedRustSleep(std::uint64_t seconds,std::uint32_t nanos) {
#ifdef _MSC_VER
  auto caller = reinterpret_cast<std::uintptr_t>(_ReturnAddress());
#else
  auto caller = reinterpret_cast<std::uintptr_t>(__builtin_return_address(0));
#endif
  const auto rva = caller >= imageBase ? caller-imageBase : 0;
  if(rva == azs::timerSleepReturnRva && seconds == 0 && nanos == azs::simulationSleepNs) {
    InterlockedCompareExchange(activeStatus,3,2);
    nanos = azs::timerSleepNanoseconds(rva,seconds,nanos,getSpeed());
  }
  originalSleep(seconds,nanos);
}
}

bool prepareTimerFix(LONG (*readSpeed)(),volatile LONG* active) {
  auto* base = reinterpret_cast<unsigned char*>(GetModuleHandleW(nullptr));
  if(!base || sizeof(void*) != 8) return false;
  auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
  if(dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew <= 0 || dos->e_lfanew > 4096) return false;
  auto* pe = reinterpret_cast<const IMAGE_NT_HEADERS64*>(base+dos->e_lfanew);
  if(pe->Signature != IMAGE_NT_SIGNATURE || pe->FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64 ||
      pe->FileHeader.TimeDateStamp != 0x6ae26a28 ||
      pe->OptionalHeader.SizeOfImage != 0x08bf3000 || !exactFileHash()) return false;
  constexpr unsigned char sleepStart[] = {0x55,0x41,0x57,0x41,0x56,0x56,0x57,0x53,0x48,0x83,0xec,0x48,0x48,0x8d,0x6c,0x24,0x40,0x89,0xd6,0x48,0x89,0xcf,0x41,0x89};
  constexpr unsigned char caller[] = {0x31,0xc9,0xba,0x40,0x78,0x7d,0x01,0xe8,0x48,0x89,0x48,0x00,0x90};
  constexpr unsigned char thunk[] = {0xe9,0x2b,0x42,0x00,0x00};
  if(!matches(base,0x06022f40,sleepStart,pe->OptionalHeader.SizeOfImage) ||
      !matches(base,0x05b963bc,caller,pe->OptionalHeader.SizeOfImage) ||
      !matches(base,0x0601ed10,thunk,pe->OptionalHeader.SizeOfImage)) return false;
  imageBase = reinterpret_cast<std::uintptr_t>(base);
  getSpeed = readSpeed;
  activeStatus = active;
  return MH_CreateHook(base+0x06022f40,reinterpret_cast<LPVOID>(hookedRustSleep),
      reinterpret_cast<LPVOID*>(&originalSleep)) == MH_OK;
}
