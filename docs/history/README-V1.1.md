# AZS Speed Controller v1.1 — Windows source candidate

This is a **source candidate**, not a compiled or Windows-tested release. Keep the validated v1.0 ZIP as a backup.

## Changes
- Fifth `*x` speed button, configured with a slider from x1 to x25 in 0.5 increments. The existing x1/x2/x5/x10 buttons and F6-F11 shortcuts remain.
- Settings panel (click **S** or right-click > Settings) with opacity slider (30%-100%), custom speed, language selection, overlay lock and always-on-top option.
- Language defaults to Windows UI language: French if Windows UI language is French, otherwise English. Manual English/French overrides are saved. **This is an initial bilingual implementation, not a complete localization**; some status strings and controls remain in English.
- Settings persist in `AZSSpeedController.ini` next to the EXE.
- The hook's previous x10 cap is raised to x25. Above x10 is experimental and requires confirmation on first use per session.

## Build
From a Visual Studio 2026 Developer PowerShell in this folder:

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release
```

Run `build\Release\AZSSpeedController.exe` with `AZSSpeedHook.dll` beside it. To generate the portable ZIP:

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\package-release.ps1
```

The packaging script creates a v1.1 ZIP. Test the binaries and clean-PC portability before publishing.

## Test checklist
1. Game connection, x1/x2/x5/x10, F6-F11, close/reset to x1.
2. Custom x1.5/x12.5/x25, first-use warning above x10; verify stability and return to x1.
3. Opacity slider at 30%, 65%, 100%; settings persistence.
4. Windows French and English default; manual overrides.
5. Lock position, always-on-top, multi-monitor placement, compact mode, taskbar icon.

Note: normal shutdown attempts to reset speed; forced termination/crashes do not guarantee reset. This utility loads a DLL into the game's process and should only be used with the developer's permission.
