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

# Changelog

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
