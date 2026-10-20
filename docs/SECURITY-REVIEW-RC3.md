# AZS Speed Controller — security review and RC3 handoff

## Status

This is a **source-only RC3 candidate**, not a Windows-tested binary and not a malware-free certification. Do not replace the published RC2 release until the maintainer rebuilds and verifies this candidate.

## Why antivirus engines may flag this program

- Controller injects a DLL using OpenProcess, VirtualAllocEx, WriteProcessMemory, CreateRemoteThread and LoadLibraryW.
- DLL hooks QueryPerformanceCounter, GetTickCount, GetTickCount64 and timeGetTime via MinHook.
- Uncommon unsigned executables, remote thread injection and memory patching can trigger heuristic/ML detections. These facts **do not prove** any specific VirusTotal alert is a false positive.

## RC3 changes relative to uploaded RC2

- Refuse to inject when more than one accessible `AzerothSurvivors.exe` process is present; verify the image basename with QueryFullProcessImageNameW rather than relying only on the process snapshot name.
- Validate that the adjacent hook file is a regular file and recognized by Windows as a 64-bit binary before injecting.
- Generate per-binary SHA256 values in `dist/SHA256-BINARIES.txt` in addition to the ZIP checksum.
- Keep existing injection and time-hook architecture unchanged; no obfuscation, packing, signature-evasion or antivirus bypass.

## Known limitations and release gates

1. Build with MSVC x64, test launch/injection, overlay, hotkeys, speed reset and reconnect.
2. Verify the DLL architecture check on the target Windows version. If GetBinaryTypeW rejects a valid DLL, stop and investigate; do not silently bypass it.
3. Verify behavior with multiple game instances and with inaccessible game processes.
4. Run Microsoft Defender on both new binaries; submit any disputed detections via https://www.microsoft.com/en-us/wdsi/filesubmission and retain case identifiers. **Only Microsoft can adjudicate its detection.**
5. Examine VirusTotal behavior reports for unanticipated network, persistence, credential, registry or file activity. A static review alone cannot prove absence.
6. Compare SHA256-BINARIES.txt with the exact EXE/DLL shipped in the ZIP; publish both checksum files and clear documentation.
7. Test on another Windows x64 machine and check whether the game permits third-party process modification.
8. No Windows build/runtime, full dependency audit, or independent sandbox execution was possible in the current Linux environment.

## How to build and package on Windows

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\scripts\package-release.ps1
Get-Content .\dist\SHA256-BINARIES.txt
```

Use the one-shot execution-policy override only after reviewing the local script. It does not disable antivirus.

## Important

A lower VirusTotal score is **not** the security acceptance criterion. A legitimate implementation, source transparency, reproducible outputs, independent testing and vendor review matter more.
