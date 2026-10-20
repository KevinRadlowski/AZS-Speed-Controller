# RC3 handoff — source inspection, 21 October 2026

## Scope and evidence

Inspected the user-supplied AZS-Speed-Standalone(4).zip. This is a **static source/package review on Linux**, not a Windows compilation, runtime validation, malware analysis or vendor clearance. Existing RC3 executables have not been rebuilt or modified.

## Confirmed implementation

- CMake 3.21+, C++17, x64 MSVC runtime configured statically, MinHook v1.3.4 retrieved using CMake FetchContent.
- Controller locates `AzerothSurvivors.exe`, verifies process image name, checks for multiple accessible instances, loads adjacent 64-bit `AZSSpeedHook.dll` using OpenProcess / VirtualAllocEx / WriteProcessMemory / CreateRemoteThread / LoadLibraryW.
- DLL hooks QueryPerformanceCounter, GetTickCount, GetTickCount64, and timeGetTime with MinHook. These behaviors are inherently sensitive to AV heuristics and warrant disclosure.
- Source has no evident outbound network client in the inspected controller/hook paths. This is **not proof** of absence of malicious behavior in shipped binaries or dependencies.
- Release packaging script emits ZIP and separate SHA-256 files. Workflow now uploads both checksum manifests.

## Issues requiring action or verification

1. **Release gate:** Do not represent submitted Microsoft samples as cleared. Wait for separate Microsoft verdicts for EXE and DLL. VirusTotal scores alone cannot establish safety.
2. **Binary/source correspondence:** The bundled RC3 binaries were not compiled here. On Windows rebuild from clean checkout and compare against the released artifacts; note that builds may not be byte-identical without reproducible-build controls.
3. **Time-hook behavior:** `scaleClock` uses one shared mutex for four clock families. Its unsigned arithmetic, 32-bit timer rollover, frequency/clock transitions, and performance under heavy concurrent calls require in-game stress testing.
4. **Injection timeout:** When remote LoadLibraryW thread times out, remote path allocation intentionally remains to avoid use-after-free. Investigate repeated failures and possible accumulated allocations.
5. **IPC readiness:** `connectMap` maps shared memory, but the visible controller path does not check `shared->active` before treating mapping as connected. Check startup races and stale/incomplete hook initialization.
6. **Process matching:** Only *accessible* same-named processes are counted. An inaccessible duplicate may go unnoticed. Test multi-instance and permission boundaries.
7. **DLL check:** `GetBinaryTypeW` confirms binary class, not cryptographic authenticity or complete DLL trust. The neighboring DLL may be replaced; signature verification is not implemented.
8. **Installer/location:** Configuration is saved beside EXE; write access may fail under Program Files. Test standard-user extraction and portable directories.
9. **Localization and UI:** Manual tests across 14 languages, DPI settings, hotkeys, and all secondary dialogs are still required.
10. **Supply chain:** Pin dependencies and GitHub Actions to reviewed immutable commits for stronger release integrity; check MinHook licensing and attribution.

## Suggested release procedure

- Freeze source commit and record commit SHA.
- On a trusted Windows x64 runner, configure and build Release; retain full build logs.
- Test with and without the game, multiple launches, every speed setting, hotkeys, clean shutdown, restart, 14 languages, DPI and a separate clean Windows PC.
- Review Defender results and Microsoft submissions for **each exact binary SHA-256**.
- Regenerate `dist` with `scripts/package-release.ps1`, verify the EXE/DLL hashes against the ZIP, attach `SHA256SUMS.txt` and `SHA256-BINARIES.txt` to a GitHub pre-release.
- Avoid telling players to disable antivirus, add broad exclusions, or ignore security warnings.

## Files deliberately excluded from public source snapshot

`.git/`, `build/`, `dist/`, local settings and prior release binaries. Original uploaded archive remains the source of truth for its preexisting Git state.
