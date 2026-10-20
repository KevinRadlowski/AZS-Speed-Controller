# Localization

The interface currently contains 14 languages: English, French, Polish, Czech, German, Brazilian Portuguese, Spanish, Romanian, Turkish, Russian, Ukrainian, Simplified Chinese, Japanese and Korean. `Automatic (Windows)` is a selection mode, not a fifteenth translation.

`src/controller.cpp` currently contains two translation tables:

- `localized[][14]`: original main-overlay and settings strings.
- `auxLocalized[][14]`: shortcut editor, speed-profile panel, and related action labels.

**Column order is identical in both tables**: EN, FR, PL, CS, DE, PT-BR, ES, RO, TR, RU, UK, ZH-CN, JA, KO. All strings are wide UTF-16 literals; MSVC uses `/utf-8` to read the source.

### Translation review checklist

- Change the dropdown to every language in turn, then open settings, shortcuts and speed profiles.
- Confirm every label, instruction and error message appears in the selected language.
- Confirm accented Latin, Cyrillic, Chinese, Japanese and Korean glyphs display without replacement characters.
- Test long German, Polish, Portuguese and Russian labels for truncation at 100%, 125% and 150% Windows scaling.
- Verify all translations with a fluent reviewer before claiming native-quality localization. Current strings are initial translations, not professionally reviewed.
- Avoid changing translation table ordering without updating `resolvedLanguage()`.

**Known debt:** Some original context-menu messages use a partial lookup with an English fallback. A future refactor should consolidate all UI strings under stable named keys and automated completeness checks.
