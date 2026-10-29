AZS SPEED CONTROLLER v1.3.1 — WINDOWS x64
=======================================
Unofficial community utility for Azeroth Survivors.

QUICK START
1. Close the game and the previous controller completely before updating.
2. Extract the entire ZIP into a writable folder.
3. Keep AZSSpeedController.exe and AZSSpeedHook.dll together.
4. Start the game, then AZSSpeedController.exe.
5. The chrono status should change from armed/pret to call observed/appel detecte.
   This status confirms targeting, not a measurement of the speed achieved.

CHRONO FIX
The maintainer confirmed the preview works through x25 on the tested 0.127.0
Windows game executable. This release preserves the same hook implementation.
Only that exact EXE is recognized. Other builds show "Chrono fix unavailable"
and retain classic clock acceleration, whose game timer may plateau.
The game EXE on disk is not modified. Restart the game when changing versions.

CONTROLS
F6 = x1, F7 = x2, F8 = x5, F9 = x10
F10 = compact / expanded, F11 = minimize / restore
Custom speed up to x25, profiles and remappable shortcuts: settings / right-click.
Normal controller exit requests x1. Forced termination may not reset the speed.
Settings: AZSSpeedController.ini beside the EXE.

FRANCAIS
Fermez completement jeu et ancien controleur avant la mise a jour.
Extrayez le ZIP, puis lancez le jeu et AZSSpeedController.exe.
Le correctif chrono a ete confirme jusqu'a x25 sur l'EXE 0.127.0 teste.
Si le statut indique "Correctif chrono indisponible", ce correctif n'est pas actif.
F6 remet la vitesse normale. Aucun remplacement de l'EXE du jeu n'est necessaire.

LIMITS
Solo test on one Windows setup; coop and independent clean-machine testing
are unvalidated. Higher speed requires more CPU and may destabilize the game.
No new antivirus scan has been performed for these binaries. The DLL injection
and timing hooks may trigger security software; old RC3 reports do not apply to
these new binaries. Do not disable protection just to run the tool.
No installer, updater or telemetry. Provided as-is; see LICENSE and release notes.
