# AZS Speed Controller v1.2.0 — first milestone (source candidate)

Based on v1.1.5 controller and v1.1.3 project scaffolding.

## Features
- F6 / F7 / F8 / F9: x1 / x2 / x5 / x10
- F10: compact overlay
- F11: minimize / restore overlay
- **F12: emergency speed reset to x1**, including while overlay is minimized
- Settings icon is a vector gear, no additional image needed
- Existing opacity, translations, custom speed and saved settings retained

## Build
```powershell
cmake -S . -B build -A x64
cmake --build build --config Release
```

Close the previous controller and game before testing the new binaries, to avoid keeping an older DLL injected in the running game.

This is a source candidate, not a Windows-tested binary. F12 may conflict with another program's global shortcut.
