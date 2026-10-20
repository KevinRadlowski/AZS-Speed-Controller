# Windows acceptance checklist (v1.2.2)

- [ ] Clean `cmake -S . -B build -A x64` and `cmake --build build --config Release` succeed.
- [ ] Controller and DLL start from a fresh extracted portable ZIP without developer tools.
- [ ] ×1, ×2, ×5, ×10, custom ×1–×25 function; warning appears above ×10.
- [ ] F6 returns to ×1; F7–F11 work in game focus and while overlay is minimized.
- [ ] All 11 actions can be rebound, cleared, saved, and restored on relaunch.
- [ ] Duplicate shortcuts and OS-reserved shortcuts are rejected without losing prior binding.
- [ ] Profiles can be saved with right-click and activated with left-click, including ×25 after warning.
- [ ] Profiles, hotkeys, language, opacity, and window state persist after restart.
- [ ] Compact overlay and expanded overlay work; overlay can be repositioned/locked.
- [ ] Quit/relaunch game switches offline/online without leaving old speed applied.
- [ ] Close controller at ×25 resets game to ×1.
- [ ] French accents and dropdown scrolling display correctly; spot-check every locale.
- [ ] Run Windows Defender scan, verify SHA-256, and publish source + ZIP together.

Do not mark the release stable until all relevant tests pass.

## v1.2.2 additional localization checks

- [ ] Open **Shortcuts** and **Speed profiles** in every language and confirm translated titles, buttons, action names, hints, and confirmation/error notices.
- [ ] Test reassigning and clearing a shortcut in French, Portuguese, Russian, Japanese and Chinese.
- [ ] Confirm profile right-click saves the active speed even if the custom slider shows another value.
- [ ] Check text truncation at 100%, 125% and 150% DPI scaling.
- [ ] Confirm settings language selection updates already-open auxiliary windows (reopen them if necessary).
