# Changelog

## 1.3.1 — 2026-10-29

- Fix the game timer plateau above approximately ×4 by scaling the local simulation thread's 25 ms wait. The maintainer confirmed the preview works through ×25 on the supplied Azeroth Survivors 0.127.0 Windows executable.
- Enable the targeted hook only after full EXE SHA-256, PE metadata and in-memory instruction checks. Unrecognized builds retain classic clock hooks and explicitly show that the chrono fix is unavailable.
- Show whether the chrono hook is armed or its matching call has been observed; wait for hook initialization before reporting a connection.
- Keep the game file on disk unchanged and leave unrelated Rust waits untouched.
- Update visible version labels, packaging and add a timer-policy test to Windows CI. Support static MinGW cross-builds.
- Validation: portable Windows x64 binaries cross-compiled; native timer-policy test passed; the game test was performed with the preceding preview and the release preserves its hook implementation. New MSVC CI, other-PC, coop and antivirus validation remain pending.

## Unreleased — GitHub preparation after v1.3.0-rc1 Windows test

- Preserve saved shortcut assignments when registration fails.
- Free remote DLL path allocation when the remote loader has completed, even on failure.
- Mark the hook ready only when all four clock hooks are installed and enabled.
- Add proposed MIT license, third-party notices, issue templates and GitHub publishing guide.
- Include MinHook license in the binary package.
- **Not yet rebuilt or runtime-tested on Windows; previous RC1 binary results do not validate these edits.**

## 1.3.0-rc1 — Release preparation (unverified)

- Resolve the companion DLL relative to the controller executable instead of the process working directory.
- Update packaging version and produce a SHA-256 checksum file.
- Extend Windows CI to produce the portable candidate ZIP and checksum artifact.
- Add release gate and documentation updates.
- Windows compilation, runtime behavior, and translation quality require validation.

## 1.2.3 — UI refresh and repository preparation (candidate)

- Refresh all open owner-drawn windows immediately on language change.
- Clear transient shortcut feedback when switching locale.
- Add release checklist and Windows build CI template.
- Move historical README versions into docs/history.
- Source-only candidate; MSVC build and runtime tests pending.

## [1.2.2] - Source candidate (unreleased)

- Localize shortcut editor and speed-profile panel in all 14 supported languages.
- Fix source declaration order for utility-window rendering functions.
- Save the applied speed (rather than the inactive custom slider value) on profile right-click.
- Add source audit, localization notes, and updated build/release documentation.
- No Windows build or runtime verification performed in this environment.


## [1.2.1] - Source candidate (unreleased)
- Added remappable shortcuts for all supported actions, with INI persistence and collision checks.
- Added three persistent speed profiles and an in-app profiles panel.
- Added shortcuts/profiles access from settings and overlay context menu.
- Removed redundant F12 shortcut; retained F6 → ×1.
- Updated CMake version and release ZIP name; added GitHub-ready documentation and ignore rules.
- **Unverified**: Windows build and game runtime tests are required before release.

## [1.2.0]
- Added a gear settings icon and version label; compact/hide functionality.

## [1.1.x]
- Opacity, custom speed up to ×25, translations, dropdown, lock position, always-on-top, reconnect handling.
