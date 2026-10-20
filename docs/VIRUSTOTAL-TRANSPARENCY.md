# Antivirus / VirusTotal transparency (v1.3.0 RC2)

AZS Speed Controller uses Windows process access and DLL injection to load its companion DLL into the local Azeroth Survivors process. The DLL uses MinHook to intercept Windows timing APIs. This behavior resembles techniques used by malware, so heuristic antivirus engines may flag the program even when it is used for its documented purpose.

## Maintainer-reported RC1 scan results (2026-10-20)

- `AZSSpeedController.exe`: 4 / 71 VirusTotal engines flagged the submitted sample.
- `AZSSpeedHook.dll`: 2 / 70 engines flagged the submitted sample.
- Release ZIP: 1 / 68 engines flagged the submitted sample.
- Maintainer's targeted Microsoft Defender scans reported zero threats.

These results refer **only to RC1 samples**, not to RC2. Detection counts may change after reanalysis. Neither low detection counts nor a clean Defender scan prove a binary is safe.

RC1 VirusTotal sample identifiers (SHA-256):

- EXE: `02cdcd17f6fa2963b4ce62fbe1db5c1aa79d13ee376f704ad1de7006ecd58dde`
- DLL: `5b7f006444497e22f16be4581365284e82901a79381a5efde3ba63207eb612a3`
- ZIP: `38c1495c257609f73453ec4400529b602a6818f2763de25a3f6b5449ddaf689d`

## Why some behavior labels appear

- **Process injection / shared modules**: the controller loads `AZSSpeedHook.dll` into the game process.
- **Execution-flow modification**: MinHook redirects timing API calls.
- **Input capture**: controller uses Windows hotkeys and keyboard state to support remappable shortcuts; no keystroke logging mechanism was found in the reviewed application sources.
- **Impair defenses / anti-analysis**: sandbox labels are not proof of intent. Investigate the specific events and dependencies rather than assuming the labels are benign or malicious.
- **Network activity**: no application networking mechanism was found in the reviewed C++ sources, and the supplied sandbox screenshots showed no network communications. Neither observation proves absence under all conditions.

## Verification and distribution

1. Obtain binaries from the official GitHub release and verify `SHA256SUMS.txt` against the downloaded ZIP.
2. Review the public sources and build workflow. Published binaries are not automatically proven identical to a local build; reproducible-build verification has not been established.
3. Keep Microsoft Defender enabled. Do not whitelist or disable protection solely to run this software.
4. Use only with game versions and environments where modification is permitted; avoid online/competitive use.
5. Report suspicious behavior through the repository security policy.

## RC2 validation status

RC2 contains a change to hook setup and rollback. It is a **source candidate** until CI and local Windows/game testing succeed. RC1 VirusTotal results must not be presented as RC2 scan results.
