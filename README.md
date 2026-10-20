# Azeroth Survivors Speed Controller

Unofficial **portable Windows x64** speed controller for *Azeroth Survivors*. Community-developed; not affiliated with the game developers. Distribution should follow the game developers' permission and any applicable game rules.

> **v1.3.0-rc1 source candidate:** the earlier RC1 compiled and ran on the maintainer's Windows PC, including a portable-folder test. Additional source fixes in this package have **not** yet been rebuilt or retested. Clean-PC validation remains pending.

## Features

- Speed buttons: ×1, ×2, ×5, ×10, and adjustable custom ×1–×25 (×10+ experimental).
- Configurable global shortcuts for **every action**: speed presets, custom speed, compact mode, show/hide, settings, and three profiles.
- Three saved speed profiles; left-click to apply, right-click to save the current speed.
- Compact overlay, opacity, position lock, always-on-top, multilingual interface.
- Settings stored alongside the EXE in `AZSSpeedController.ini`; no installer.
- Game detection and reconnection; speed reset on normal exit.

## Usage

1. Download an official release archive from the project's GitHub Releases (once published).
2. Extract **both** `AZSSpeedController.exe` and `AZSSpeedHook.dll` into the same writable folder.
3. Launch the game and controller; no installation required.
4. Click the gear icon for settings. Open **Keyboard Shortcuts** to remap any action. Click a row and press a combination; `Delete` clears it, `Esc` cancels. Duplicate or OS-reserved combinations are rejected.
5. Open **Speed Profiles** to apply or save three speed presets. Right-click a profile to save the active speed, left-click to apply it.
6. Right-click the overlay for a context menu with settings, profiles and shortcuts. In compact mode, click the status area to return to ×1.

### Default keys

| Action | Default |
| --- | --- |
| ×1 | F6 |
| ×2 | F7 |
| ×5 | F8 |
| ×10 | F9 |
| Compact | F10 |
| Show/hide (minimize/restore) | F11 |
| Custom speed, settings, profiles 1–3 | Unassigned |

F12 is intentionally **not** assigned. Configure any action from the shortcuts panel.

## Build from source

Visual Studio 2022/2026 with C++ workload, Windows SDK, CMake, and network access to fetch MinHook v1.3.4:

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release
powershell -ExecutionPolicy Bypass -File .\scripts\package-release.ps1
```

Release output is `build\Release\AZSSpeedController.exe` and `AZSSpeedHook.dll`. The packaging script creates a portable ZIP under `dist/` and a `SHA256SUMS.txt` file. **No built binaries are included in this source ZIP.**

## Security and transparency

This program injects its companion DLL into `AzerothSurvivors.exe` to modify the process clock. This may trigger antivirus heuristics. Only run binaries from a source you trust; inspect/build the code yourself where possible. Never disable antivirus protection just to run this tool. There is no network updater or telemetry in the reviewed source. Please report suspected security issues privately (see [SECURITY.md](SECURITY.md)).

## Compatibility and limitations

Windows x64 only. Overlay behavior can vary with exclusive fullscreen, focus and other overlays. Speed above ×10 may destabilize the game. Shortcut registration may fail if another program or Windows already uses the combination; a failed shortcut should be changed in the panel. This tool has not been validated on a clean Windows machine.

## Localization

The UI supports 14 languages, including the shortcut editor and speed profiles. See [LOCALIZATION.md](LOCALIZATION.md) for language coverage, translation review, and remaining fallback limitations.

## Code audit

See [CODE-AUDIT.md](CODE-AUDIT.md) for architecture, known risks and the release checklist. The v1.3.0-rc1 changes have not been compiled in this environment.

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) and [CHANGELOG.md](CHANGELOG.md). The repository includes a proposed **MIT license**; the maintainer must confirm ownership and approve its copyright attribution before publishing. See [THIRD-PARTY-NOTICES.md](THIRD-PARTY-NOTICES.md).


Release acceptance criteria: [docs/RELEASE-CHECKLIST.md](docs/RELEASE-CHECKLIST.md).

## Publishing

See [GitHub publishing guide](docs/PUBLISH-GITHUB.md) and [pre-release notes](docs/GITHUB-RELEASE-DRAFT.md). Screenshots can be added from actual in-game captures; the included preview should be checked against the current interface.
