# Release readiness checklist — v1.3.0-rc1

## Must pass before publishing binaries

- [ ] Rebuild updated source with Windows x64 MSVC Release (previous RC1 build passed before these changes).
- [ ] Launch with and without game running; reconnect after game restart.
- [ ] Check speed x1/x2/x5/x10 and custom speeds; confirm timers and waves.
- [ ] Change language while Settings, Shortcuts and Profiles are all open.
- [ ] Test all 14 locales: accents, glyph coverage, truncation and scrolling.
- [ ] Test hotkey remapping, duplicates, rejected Windows hotkeys, Delete and Escape.
- [ ] Test profiles: right-click saves active speed, left-click applies speed.
- [ ] Test compact mode, hide/show, locked position, opacity and always-on-top.
- [ ] Test 100%, 125%, 150%, 200% display scaling and multi-monitor placement.
- [ ] Test app exit while game is running, and relaunch after an unclean exit.
- [ ] Test on a clean Windows machine without Visual Studio installed (deferred; explicitly disclose for RC1).
- [ ] Inspect security alerts without instructing users to disable protection.
- [ ] Maintainer approves proposed MIT license and copyright attribution; verify third-party notices.
- [ ] Publish release EXE + DLL + README + SHA-256 sums from a clean build.

## Known audit issues, not claimed fixed in v1.3.0-rc1

- Injection cleanup improved for completed failure paths; timeout path remains intentionally allocated to avoid a use-after-free; diagnostics still limited.
- Hook readiness now requires all four hooks; failure reporting remains limited.
- Hotkey registration now preserves saved bindings; UI and rebinding regression test still needed.
- Configuration writes beside the EXE can fail in protected folders.
- Large controller.cpp needs modularization and automated unit tests.
- Native-speaker review of all translations and accessibility testing.
- Exclusive fullscreen overlay compatibility is not guaranteed.
