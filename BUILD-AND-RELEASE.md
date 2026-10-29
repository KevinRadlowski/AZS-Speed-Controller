# Build and release v1.3.1 (Windows x64)

Requirements: Visual Studio with Desktop development with C++, Windows SDK, CMake, Git and network access for MinHook v1.3.4.

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
powershell -ExecutionPolicy Bypass -File .\scripts\package-release.ps1
```

Binaries: `build/Release/AZSSpeedController.exe`, `build/Release/AZSSpeedHook.dll`.
Portable ZIP, archive checksum and binary checksums: `dist/`.
Attach the ZIP, SHA256SUMS.txt and SHA256-BINARIES.txt to the v1.3.1 GitHub release using `docs/RELEASE-NOTES-v1.3.1.md` as its body. Publish only from the exact reviewed commit and do not replace artifacts of an existing tag.

The supplied cross-build is MinGW-w64, not MSVC. Rebuilding with MSVC will produce different hashes. New Windows CI must run before claiming its success; the maintainer game test applies to the preview hook, whose implementation is preserved by this release.
