# v1.3.1 release status — 2026-10-29

- Windows x64 chrono preview: maintainer confirmed game timer acceleration through ×25 on the supplied game 0.127.0 EXE.
- Release preserves the tested hook implementation. Controller title/version labels updated to v1.3.1.
- Release EXE/DLL cross-built with MinGW-w64 13 POSIX, static GCC/pthread runtimes; MinHook v1.3.4 pinned.
- Timer policy, uploaded-EXE signatures and PE imports checked. CI now runs the policy test in Release configuration with assertions enabled.
- Windows/MSVC CI for this release, independent clean-machine tests, coop tests and new antivirus scan: pending, not claimed.
- Exact EXE hash plus in-memory signatures gate the chrono hook; unsupported builds retain classic timing hooks.
- Linux prototype remains separate and has not received this Windows-only fix.
- GitHub publication requires authenticated repository access. A prepared archive/commit does not mean a public release exists.
