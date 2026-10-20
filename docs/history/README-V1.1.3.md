# AZS Speed Controller v1.1.3 — source update

Updates: complete UI text dictionary for English, French, Polish, Czech, German, Brazilian Portuguese, Spanish, Romanian, Turkish, Russian, Ukrainian, Simplified Chinese, Japanese, Korean. Automatic Windows UI language resolution. Dark custom scrollable language dropdown. UTF-8 source compile flag. Version label in overlay/settings. Existing speed engine untouched.

This is a source candidate; **not compiled or runtime tested on Windows**. Keep a backup of your working v1.1.2 and test all language options. Translations are initial drafts and may benefit from native-speaker review. The original language names in the dropdown are intentionally displayed in their own languages.

Copy src/controller.cpp and CMakeLists.txt into your project (or use the entire source tree), then:

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release
```

Close the game and controller before testing the new DLL; start the game and then controller again. Do not publish before testing.
