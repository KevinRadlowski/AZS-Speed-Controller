# v1.3.0-rc2 release gate

## Build and packaging

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release
.\scripts\package-release.ps1
Get-FileHash .\dist\AZS-Speed-Controller-v1.3.0-rc2-Windows-x64.zip -Algorithm SHA256
```

Verify the ZIP includes `AZSSpeedController.exe`, `AZSSpeedHook.dll`, `README.txt`, `LICENSE.txt` and `MINHOOK-LICENSE.txt`. Verify the hash equals the line in `dist/SHA256SUMS.txt`.

## Manual runtime checks (Windows game machine)

- Start controller without game; verify disconnected state, no crash.
- Start game; verify controller connects and reports ready.
- Check x1, x2, x5, x10, custom speeds; check game stability and return to x1.
- Verify all hotkeys with controller focused and game focused, including remapped hotkeys and persistence.
- Open settings, profiles and shortcuts; switch languages while dialogs remain open.
- Close and restart game/controller; verify reconnection and no orphaned UI.
- If hook initialization fails, readiness should remain false. **Restart the game before retrying**: a DLL that failed initialization can remain loaded and Windows may not call DllMain again on repeated LoadLibrary requests.
- Test on a separate clean Windows x64 machine if possible.

## Security checks

- Scan **new RC2** EXE, DLL and ZIP independently with Defender and VirusTotal, if desired.
- Review exact sandbox events for anti-analysis and defense-impairment labels.
- Check hashes of release artifacts and confirm that the uploaded files match the CI-produced ZIP.
- Do not claim a zero-risk or malware-free certification based on antivirus scores.

## Release sequence

1. Commit and push RC2 sources.
2. Check the GitHub Actions Windows build and download its artifact.
3. Test that exact artifact locally.
4. Create tag `v1.3.0-rc2` only after successful validation.
5. Publish RC2 as a **pre-release**, with its own checksum and updated scan disclosures.
6. Keep RC1 intact as a prior pre-release.
