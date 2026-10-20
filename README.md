# Azeroth Survivors Speed Controller

Unofficial **portable Windows x64** speed controller for *Azeroth Survivors*. Community-developed; not affiliated with the game developers. Distribution should follow the game developers' permission and any applicable game rules.

> **Current pre-release: v1.3.0-rc3.** The maintainer confirmed local Windows x64 operation after the DLL validation fix, and GitHub Actions Windows build #3 passed at commit `1cccc8a`. Independent clean-PC testing and reproducible-build verification remain pending.

## Features

- Speed buttons: ×1, ×2, ×5, ×10, and adjustable custom ×1–×25 (×10+ experimental).
- Configurable global shortcuts for **every action**: speed presets, custom speed, compact mode, show/hide, settings, and three profiles.
- Three saved speed profiles; left-click to apply, right-click to save the current speed.
- Compact overlay, opacity, position lock, always-on-top, multilingual interface.
- Settings stored alongside the EXE in `AZSSpeedController.ini`; no installer.
- Game detection and reconnection; speed reset on normal exit.

## Usage

1. Download an official pre-release archive from [GitHub Releases](https://github.com/KevinRadlowski/AZS-Speed-Controller/releases). Read the antivirus disclosure below before running it.
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

**Current RC3 VirusTotal results (2026-10-20): EXE 6/61, DLL 2/70, ZIP 2/67. Microsoft Defender flags the RC3 EXE as `Trojan:Win32/Wacatac.B!ml`.** These are not confirmed false positives. The controller injects its companion DLL into `AzerothSurvivors.exe` and hooks Windows timing APIs, which may trigger antivirus heuristics. **Do not disable antivirus protection or bypass security warnings just to run this tool.** Read the full [VirusTotal reports, exact hashes, detection names and safety guidance](docs/VIRUSTOTAL-TRANSPARENCY.md). No network updater or telemetry was identified in the reviewed application source; this does not establish binary safety. Please report suspected security issues privately (see [SECURITY.md](SECURITY.md)).

## Compatibility and limitations

Windows x64 only. Overlay behavior can vary with exclusive fullscreen, focus and other overlays. Speed above ×10 may destabilize the game. Shortcut registration may fail if another program or Windows already uses the combination; a failed shortcut should be changed in the panel. This tool has not been validated on a clean Windows machine.

## Localization

The UI supports 14 languages, including the shortcut editor and speed profiles. See [LOCALIZATION.md](LOCALIZATION.md) for language coverage, translation review, and remaining fallback limitations.

## Code audit

See [CODE-AUDIT.md](CODE-AUDIT.md) for architecture, known risks and the release checklist. The current RC3 was built and tested on the maintainer's Windows PC and passed the GitHub Actions Windows build; a full independent security audit and reproducible-build verification have not been completed.

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) and [CHANGELOG.md](CHANGELOG.md). The repository includes an MIT license; the maintainer is responsible for confirming ownership, copyright attribution, and third-party licensing compliance. See [THIRD-PARTY-NOTICES.md](THIRD-PARTY-NOTICES.md).


Release acceptance criteria: [docs/RELEASE-CHECKLIST.md](docs/RELEASE-CHECKLIST.md).

## Publishing

See [GitHub publishing guide](docs/PUBLISH-GITHUB.md) and [pre-release notes](docs/GITHUB-RELEASE-DRAFT.md). Screenshots can be added from actual in-game captures; the included preview should be checked against the current interface.
