# Follow-up source changes (post Windows-tested RC1)

See `docs/RELEASE-STATUS.md`. The latest source edits address shortcut preservation, remote allocation cleanup after completed failures, and hook readiness, but have **not** been compiled or tested on Windows. Historical findings below refer to previous revisions.

# Static audit — v1.2.3 candidate

The locale-refresh bug was confirmed in the language popup: only the main overlay and settings window were invalidated. This candidate invalidates the shortcuts and profiles windows too. No runtime tests or Windows compilation were possible in this environment.

See [release checklist](docs/RELEASE-CHECKLIST.md) for the remaining priority issues. The v1.2.2 audit findings remain applicable unless explicitly addressed.

# Source audit — v1.2.2 candidate (2026-10-20)

Scope: static review of `src/controller.cpp`, `src/hook.cpp`, CMake, packaging and repository documentation. **Not compiled or executed on Windows in this environment.** This is not a security certification.

## Changes in this candidate

- Localized the new shortcut editor and speed-profile panel in all 14 existing interface languages, including action labels, instructions, empty shortcut state, confirmation/error messages, and the settings buttons.
- Added forward declarations for renderer and controller functions used by the utility panels before their definitions. Without these, the original uploaded source was at risk of failing MSVC compilation.
- Corrected profile right-click saving to use the **currently applied speed**, rather than an unselected custom-slider value.
- Updated source/package version to 1.2.2.

## Architecture

- `src/controller.cpp`: Win32 overlay, GDI rendering, INI preferences, hotkey registration, game process detection, remote DLL loading, shared-memory speed controls.
- `src/hook.cpp`: MinHook interception of `QueryPerformanceCounter`, `GetTickCount`, `GetTickCount64`, `timeGetTime`; maintains virtual clocks with adjustable scaling.
- `CMakeLists.txt`: MSVC x64 builds, MinHook v1.3.4 fetched from GitHub.
- `scripts/package-release.ps1`: copies EXE/DLL and a player README into the release ZIP.

## Findings and recommendations

### High priority before public release

1. **Remote injection and trust:** DLL loading via `CreateRemoteThread`/`LoadLibraryW` will trigger some security heuristics. Publish transparent source, hashes, and reproducible build instructions. Never suggest disabling antivirus. The tool is unofficial and should be distributed only where permitted.
2. **Injection timeout and memory:** `inject()` does not free its remote path allocation on all failure paths. If the remote thread is still running it must not be freed prematurely, but completed failures should be cleaned up safely.
3. **Hook initialization and error reporting:** `hookOne()` ignores failures, and the DLL can advertise `active=1` even if hooks fail. Report the number/status of successfully installed hooks before marking the controller connected.
4. **Hotkey failure handling:** `registerShortcuts()` removes a shortcut from the in-memory configuration when Windows rejects it. On startup this can silently alter the user-visible binding; add a separate registration-state array and preserve the saved preference. Rebinding failure should not discard unrelated shortcuts.
5. **Build verification:** Run an actual MSVC Release build, and clean-machine test both EXE and DLL. The provided archive includes old build artifacts but no verified binary for this patch.

### Medium priority

6. **Maintainability:** `controller.cpp` is a large single translation unit. Separate localization, hotkeys, Win32 UI, settings persistence, game connection, and shared protocol into independent modules with unit-testable interfaces.
7. **Localization quality:** The existing main panel uses a numeric-key table and `tr()` searches only 27 entries before falling back to English. Replace with named translation keys and validate all keys for every locale. New translations are initial drafts, not native-speaker certified. Long strings may be truncated in the fixed-width panels; visually test all 14 languages.
8. **Version source of truth:** Version is duplicated in CMake, UI strings and packaging. Generate a shared version header and derive archive naming from the CMake project version.
9. **Shortcut naming:** OS-generated `GetKeyNameTextW` is appropriate for local key names, but keyboard layouts and modifier handling need testing. Explain `Delete` and `Escape` semantics, and avoid hard-coded F10/F11 in tooltips after remapping.
10. **Clock correctness:** Virtual-time scaling can affect game timing and libraries differently; test rapid changes, suspend/resume, long sessions, and controller termination. Independent clock bases and integer truncation deserve tests. `Clock` delta multiplication could overflow at extreme uptimes (unlikely in normal use).
11. **Process matching:** The controller locates the first process with the matching filename. If multiple copies are running, connection selection is ambiguous.
12. **Configuration portability:** The INI is written beside the EXE, which can fail in a protected directory. Detect write errors and show a clear message.
13. **UI and accessibility:** Owner-drawn windows lack accessibility metadata and keyboard navigation. Improve focus, tab order, and screen-reader support before broader distribution.

### Repository readiness

- Keep `build/`, `dist/`, `.ini`, temporary logs and local secrets out of Git; `.gitignore` already covers common outputs.
- Add an explicit `LICENSE` after choosing the license; do not imply the project is open-source licensed until then.
- Add `CODE_OF_CONDUCT.md` if accepting broad community contributions, issue templates, and optional CI Windows build after first successful clean build.
- Preserve `SECURITY.md`, `CONTRIBUTING.md`, `CHANGELOG.md`, `TESTING.md` and a player-oriented README. Avoid contradictory historical release documents; archive old docs under `docs/history/`.

## Release gate

This ZIP is **source-only** and **unverified on Windows**. Compile with MSVC, run `TESTING.md`, inspect translations at different DPI settings, then publish an independently built release archive with SHA-256 checksums.
