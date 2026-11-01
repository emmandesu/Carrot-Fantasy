# Carrot Fantasy — English Edition

An English localization of [ggyy1122/Carrot-Fantasy](https://github.com/ggyy1122/Carrot-Fantasy), a C++ tower defense game made with Cocos2d-x 4.0 for Tongji University's 2024 Program Design Paradigms course.

**These changes localize the source UI. The inherited `Carrot.exe` and `game.zip` are original, untranslated binaries. Rebuild the game from this source to use the English interface.**

## English interface

- Main menu: Adventure, Boss Mode, Spectate, and the English game title.
- Settings: music/sound controls, ON/OFF states, and developer credits.
- Theme selection: Skyline, Desert, Jungle, Deep Sea, and Winter.
- Level selection: New Game, Load Game, Choose Level, and locked-level notices.
- Gameplay: wave counter, pause indicator, Continue, Restart, Try Again, and boss skill messages.
- Skill help: Heal, Freeze, Blast, Rapid Fire, and Slow Down, including costs and effects.
- Tower upgrade indicator: MAX.

The original three Skyline levels are playable. Other themes, Boss Mode, and Spectate remain unavailable, with English explanations of their status.

## How the translation works

[`Resources/localization/en.xml`](Resources/localization/en.xml) is an XML property list containing the English strings and a registry of text-bearing artwork. Cocos's built-in `FileUtils::getValueMapFromFile` loads it at runtime.

Most upstream text was painted directly into PNGs. [`Classes/EnglishUI.h`](Classes/EnglishUI.h) replaces those UI elements with Cocos geometry and text rendered with the bundled Arial font. Theme cards retain their landscape art with an opaque English heading covering the original title. Normal/pressed menu states and widget touch callbacks are preserved. Original artwork is kept for reference.

To edit a translation, change a string value in `en.xml`. Each artwork entry specifies its string key, pixel dimensions, font size, and optional body text. `kind=header` retains the theme illustration; `kind=plate` replaces a text-bearing element. Rendering accounts for Cocos content scale and shrinks text to fit its bounds.

Localization does not rename map layers, monster/tower identifiers, JSON fields, or save files. Existing save formats and level progression are unchanged.

## Rebuilding

This upstream repository includes `Classes/`, `Resources/`, the vendored Cocos engine, and Windows entry-point files, but no application CMake project or Visual Studio solution. It is not a complete one-command build checkout.

1. Set up a Cocos2d-x 4.0 C++ application project using a matching engine/toolchain.
2. Include the first-party `.cpp` files from `Classes/` and add `Classes/` to the include search path. `EnglishUI.h` is header-only and requires no additional source registration.
3. Package **all of `Resources/`**, including `localization/en.xml` and `fonts/arial.ttf`. `proj.win32/build-cfg.json` already copies this whole directory.
4. Use the existing `AppDelegate` and the platform entry point, then rebuild. Do not launch the inherited executable to test source changes.

Modified first-party source files use UTF-8. When building with MSVC, enable `/utf-8` for those translation units. Untouched upstream headers may still contain legacy GBK comments; choose the source-encoding settings for those files accordingly.

## Validation

Run the dependency-free catalog/resource check:

```sh
python3 tools/check_localization.py
```

It checks XML structure, missing/duplicate string keys, English-only catalog text, format placeholders, resource paths and PNG dimensions, and direct factory calls that would bypass localized artwork. The header can also be syntax-checked against the bundled Cocos headers with a C++11 compiler.

Manual checks after rebuilding:

- Visit the main menu, settings, theme carousel, level carousel, and all locked-mode notices.
- Start a new game and open the five skill help cards.
- Toggle audio, pause/resume, and 1x/2x speed; open the pause menu.
- Save through Choose Level, then use Load Game to resume.
- Check both victory and defeat screens for text fit and working buttons.

The localization helper has been syntax-checked; this fork has not been built or play-tested in a graphical game runtime.

## Original project

Original contributors: **ggyy1122, yyggcc66, and xing05188**. The upstream settings screen credits zhou, yu, and guan. The original Chinese project and design documentation is preserved in [README.zh-CN.md](README.zh-CN.md).

This fork retains the upstream game assets and third-party notices. See their original files for attribution and license information.
