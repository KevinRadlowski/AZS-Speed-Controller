# GitHub publication — RC3 (maintainer procedure)

**Do not publish as stable until security and Windows testing gates pass.** The submitted Microsoft EXE/DLL cases are independent and may still be pending.

The source archive prepared in this handoff intentionally excludes `.git`, `build` and `dist`. To preserve your existing Git history and uncommitted changes, apply reviewed edits to your original working tree rather than running `git init` or force-pushing over it.

1. Inspect `git status`, `git diff`, `git remote -v` and the current branch in your original local repository.
2. Review `docs/RC3-HANDOFF-AUDIT.md`, source changes and license/third-party notices.
3. Run `cmake -S . -B build -A x64` and `cmake --build build --config Release` on Windows with MSVC.
4. Test all release gates from `docs/RELEASE-CHECKLIST.md` and review Microsoft verdicts for each shipped binary.
5. Run `./scripts/package-release.ps1` in PowerShell and verify `dist/SHA256SUMS.txt` and `dist/SHA256-BINARIES.txt`.
6. Commit the reviewed source changes to your existing repository, then push normally. Avoid committing `build/`, `dist/`, configuration files or local secrets.
7. Create a GitHub **pre-release** for the verified RC3 build, attach the ZIP and both SHA-256 manifests, and clearly disclose DLL injection/time-hook functionality and antivirus review status.
8. Do not claim a reproducible or malware-free build until independently verified.
