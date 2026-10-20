# v1.3.0-rc1 release status — 2026-10-20

## Verified by maintainer on Windows

- CMake configured with Visual Studio 2026 and MSVC.
- Release build produced EXE and DLL.
- PowerShell packaging generated Windows ZIP and SHA256SUMS.txt.
- Archive extracted to a separate Desktop folder; maintainer confirmed it works there.

## New source changes since that successful build (NOT RETESTED)

- Failed shortcut registration no longer erases the stored key assignment.
- Remote DLL path memory is freed after completed remote-thread failures, not only on success.
- DLL reports active only after all four hooks are enabled.
- MIT license proposed, MinHook license packaging, issue templates, release documentation.

## Remaining release gates

- Rebuild and retest the **updated** sources.
- Confirm MinHook license file is packaged and CI passes.
- Owner must approve copyright/license and game permissions.
- Other-PC/clean-machine test unavailable; disclose this limitation prominently.
- All-language/DPI and failure-path testing remains incomplete.

This is a pre-release candidate, not a security certification or stable release.
