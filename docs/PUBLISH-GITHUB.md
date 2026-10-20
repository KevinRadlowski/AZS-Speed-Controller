# Publish to GitHub (maintainer steps)

1. Confirm you own the source rights and approve the proposed MIT license and copyright attribution in `LICENSE`. Verify permission to distribute a speed-modifying utility under the game rules.
2. Create a **public** repository named `AZS-Speed-Controller` on GitHub, without initializing extra files.
3. From this source directory run:

```powershell
git init
git add .
git commit -m "Prepare v1.3.0-rc1 pre-release"
git branch -M main
git remote add origin https://github.com/YOUR-USERNAME/AZS-Speed-Controller.git
git push -u origin main
```

4. Wait for the Windows build workflow to finish successfully on GitHub.
5. Rebuild this updated source locally, run the test checklist, and regenerate `dist` (the previous binary ZIP was built **before** the final code changes).
6. Compare checksums, create tag `v1.3.0-rc1`, and create a **Pre-release** in GitHub Releases using `docs/GITHUB-RELEASE-DRAFT.md`. Attach the freshly generated Windows ZIP and `SHA256SUMS.txt`.
7. Do not label it stable or claim clean-PC validation.

Screenshots: add only real captures of the current UI (e.g. `assets/`); do not present generated mockups as screenshots.
