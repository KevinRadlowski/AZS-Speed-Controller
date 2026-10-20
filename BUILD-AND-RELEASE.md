# Building and publishing AZS Speed Controller v1.3.0-rc1

## Requirements

Windows 10/11 x64, Visual Studio 2022 or 2026 with Desktop development with C++, Windows SDK, CMake, Git and network access for MinHook v1.3.4.

## Build

Open PowerShell in the project root:

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release
```

The expected output files are `build\Release\AZSSpeedController.exe` and `build\Release\AZSSpeedHook.dll`.

## Test and package

1. Test the two files side by side on a Windows machine with the game.
2. Run every applicable check in [TESTING.md](TESTING.md).
3. Build a clean-machine portable package:

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\package-release.ps1
```

4. Publish `dist/SHA256SUMS.txt` alongside the ZIP; verify the SHA-256 hash printed by the script and attach the source tag, ZIP and checksum to the GitHub release.
5. Clearly disclose that the tool injects a DLL into the game process, may be flagged by antivirus heuristics, and is unofficial.

Do not claim successful build or security validation until it has actually been performed. If the build fails, keep the complete compiler diagnostics for the maintainer.
