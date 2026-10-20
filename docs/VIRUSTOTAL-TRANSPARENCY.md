# Antivirus and VirusTotal transparency — v1.3.0 RC3

AZS Speed Controller is an unofficial Windows x64 tool for offline Azeroth Survivors. It accesses the game's process, injects its companion `AZSSpeedHook.dll`, and uses MinHook to intercept timing APIs. Antivirus products may flag these behaviors, but **the developer's intended use and heuristic detection names do not establish that any detection is a false positive**.

## RC3 release-file results (maintainer screenshots, 2026-10-20)

| File | VirusTotal detections | SHA-256 | Report |
| --- | --- | --- | --- |
| `AZSSpeedController.exe` | **6 / 61** | `5894e9dd3ead89926be19031cd1c2e27bf2d9bbce7ed054829ec7793082c9ae1` | [VirusTotal EXE](https://www.virustotal.com/gui/file/5894e9dd3ead89926be19031cd1c2e27bf2d9bbce7ed054829ec7793082c9ae1) |
| `AZSSpeedHook.dll` | **2 / 70** | `a42dbc80ec522758de2a4b91a403edb48d06c44047f4ffc4da8d332bdc8e49ef` | [VirusTotal DLL](https://www.virustotal.com/gui/file/a42dbc80ec522758de2a4b91a403edb48d06c44047f4ffc4da8d332bdc8e49ef) |
| `AZS-Speed-Controller-v1.3.0-rc3-Windows-x64.zip` | **2 / 67** | `a298a916858b12f7a3102c6115e209f3641529d040ed419cec17ed3acac6b570` | [VirusTotal ZIP](https://www.virustotal.com/gui/file/a298a916858b12f7a3102c6115e209f3641529d040ed419cec17ed3acac6b570) |

The screenshots showed these detection names:

- **EXE:** Microsoft `Trojan:Win32/Wacatac.B!ml`; CrowdStrike Falcon `Win/malicious_confidence_60% (D)`; Malwarebytes `Malware.Heuristic.108256`; MaxSecure `Trojan.Malware.300983.susgen`; SecureAge `Malicious`; Trapmine `Malicious.high.ml.score`.
- **DLL:** CrowdStrike Falcon `Win/malicious_confidence_60% (D)`; Cynet `Malicious (score: 100)`.
- **ZIP:** Malwarebytes `Malware.Heuristic.108256`; MaxSecure `Trojan.Malware.300983.susgen`.

**Microsoft Defender currently flags the RC3 EXE.** Previous EXE/DLL samples were submitted to Microsoft for review, but their status does not establish clearance for these new RC3 hashes. No vendor has been confirmed to have classified these exact RC3 files as false positives. Results may change after reanalysis.

## Why this software is sensitive to antivirus scanning

- The controller opens a game process and loads a DLL into it.
- The DLL redirects timing APIs using MinHook to adjust apparent game time.
- Global hotkeys and shared memory are used for the controller interface.

These operations overlap with techniques used by unwanted or malicious software. Source inspection and successful compilation are useful transparency measures, **not** independent proof that distributed binaries are safe.

## Before downloading or running

1. Download only from the [official GitHub releases](https://github.com/KevinRadlowski/AZS-Speed-Controller/releases).
2. Verify the release ZIP's SHA-256 against `SHA256SUMS.txt` (the hash for the ZIP listed above is specific to the locally packaged RC3 file scanned on 2026-10-20).
3. Review the source code, build workflow, and [security policy](../SECURITY.md). Reproducible-build equivalence has **not** been established.
4. **Do not disable Microsoft Defender, add broad antivirus exclusions, or bypass security warnings just to run the tool.** If your protection blocks it, leave it blocked and await vendor review or build/review the sources in an appropriately isolated environment.
5. Use only offline/single-player and where game rules permit modifications.

## Historical results

RC1 samples were previously reported as EXE 4/71, DLL 2/70, and ZIP 1/68. Those figures apply only to the older RC1 hashes and must not be confused with the RC3 results above.
