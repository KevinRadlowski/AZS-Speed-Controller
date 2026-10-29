# AZS Speed Controller v1.3.1

The game timer now keeps accelerating beyond ×5 on the tested Azeroth Survivors 0.127.0 build. The maintainer confirmed the chrono preview works through ×25 on Windows; this release keeps the same hook implementation.

- Scales the local simulation wait with the selected speed.
- Checks the exact game executable before enabling the chrono fix.
- Shows when the fix is ready and when the simulation call has been detected.
- Preserves shortcuts, custom speeds, profiles and the existing overlay.

**Updating:** close the game and previous controller completely, extract the ZIP, then launch the game and new controller. Keep the EXE and DLL together. No game files need replacing. F6 returns to ×1.

**Compatibility:** Windows x64. Chrono fix validated on one 0.127.0 executable only (SHA-256 `3ef9d74c30bb6b435d739b3d960c8535486836c540e0d8c20f98549a6f1dcbbb`). Another build uses classic clock acceleration and displays “Chrono fix unavailable”. Coop and other-PC tests are unvalidated. The Linux prototype is not included in this release.

**Validation:** cross-compiled Windows binaries and timer-policy tests passed. The preceding preview was tested in game by the maintainer through ×25. Final release changes to the controller are version labels/title only; the locally prepared MinGW DLL is byte-identical to the tested preview. GitHub release assets are rebuilt with MSVC and therefore have different hashes. The new Windows/MSVC CI has not yet run. No new antivirus scan: historical RC3 reports do not cover these files.

The tool injects its companion DLL and redirects timing functions in memory. It does not modify the game EXE on disk. Higher speeds can increase CPU use and cause instability.

---

Le chrono suit maintenant l'accélération au-delà de ×5 sur l'EXE 0.127.0 testé. Le correctif a été confirmé jusqu'à ×25 par le mainteneur.

Pour mettre à jour : fermez complètement le jeu et l'ancien contrôleur, extrayez le ZIP, puis relancez le jeu et le nouveau contrôleur. Gardez l'EXE et la DLL ensemble. F6 revient à ×1.

Le correctif reconnaît uniquement l'EXE testé : une mise à jour du jeu demandera une nouvelle vérification. Test effectué sur une seule configuration Windows ; coop et autres PC non validés. Aucun nouveau scan antivirus effectué pour cette version.
