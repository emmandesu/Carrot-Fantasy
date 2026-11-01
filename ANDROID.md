# Android port

This is a native Cocos2d-x 4.0 build of the English-translated game. Rendering uses OpenGL ES; all game assets are packaged in the APK, and the app requests no network or storage permissions.

- Package: `com.emmandesu.carrotfantasy`
- Version: `1.0.0` (code 1)
- Android: 6.0/API 23 or newer; target API 36
- Release APK architecture: ARM64 (`arm64-v8a`)
- Landscape, with the original 960 × 640 game area fitted inside the display. Wide phones have side bars so gameplay coordinates remain accurate.
- Touch input supports tower building, targeting, upgrading, and selling. The Android Back button opens the in-game pause menu, or goes back from selection/settings screens.
- Saves live in Android's app-private files directory. Choose Level saves the current game; moving the app to the background also creates a checkpoint. Use Load Game to restore it after relaunching. Uninstalling the app or clearing its data removes saves.

## Building the APK

Required: JDK 17, Python 3, CMake 3.16+ (tested with 3.31.6), Ninja, and these official Android SDK components:

```sh
sdkmanager "platforms;android-36" "build-tools;36.0.0" "ndk;23.2.8568313"
```

From the repository root:

```sh
export ANDROID_SDK_ROOT=/path/to/Android/Sdk
export JAVA_HOME=/path/to/jdk-17
export PATH="$JAVA_HOME/bin:$PATH"
python3 tools/build_android.py --sdk "$ANDROID_SDK_ROOT"
```

If CMake or Ninja is outside PATH, pass `--cmake /path/to/cmake --ninja /path/to/ninja`. The output is `android-output/Carrot-Fantasy-1.0.0-arm64-v8a.apk`.

The script builds the native code, compiles Cocos's Java bridge and AIDL, packages the assets, then signs and verifies a development APK. It creates or reuses the local Android debug keystore at `~/.android/debug.keystore`. Preserve that keystore to sign compatible updates. For public distribution, use a private release signing key instead. No keystore is committed to this repository.

The pinned NDK preserves compatibility with this older Cocos engine and its bundled static libraries. The C++ runtime is linked statically, and the app library has 16 KB ELF segment alignment.

## Android Studio project

Open `proj.android/` with JDK 17 and Gradle 8.11.1. The project uses Android Gradle Plugin 8.10.1 and CMake 3.22.1, which Android Studio can install. Its Gradle build and the command-line script share the root CMake project and game sources. The command-line build script is the build route verified for the delivered APK; the Android Studio route is supplied for development and has not been run here.

## Port-specific fixes

- Case-sensitive resource/header paths and UTF-8 source files for Android's compiler and asset manager.
- Touch-to-game coordinate conversion, with HUD controls excluded from board touches.
- Sprite ownership fixes when closing menus, upgrading, selling, and destroying towers.
- Monster ownership and unique spawn scheduler keys across overlapping waves.
- Checkpoints preserve tower/obstacle state, coins, carrot health, live monster positions/health, and pending wave spawns. Monster saves use schema version 2; original Windows monster saves are not imported.
- Save files are published using a temporary file, flush, and rename, protecting the previous complete JSON file from partial writes.
- Background checkpoints and audio suspension; single game-over screen rather than one created on every frame.

## Validation

```sh
python3 tools/check_localization.py
mkdir -p /tmp/carrot-save-test
g++ -std=c++11 -IClasses -Icocos2d/external tools/tests/save_io_test.cpp -o /tmp/carrot-save-test/check
/tmp/carrot-save-test/check /tmp/carrot-save-test
```

The APK build script verifies its signature and ZIP alignment. The native ARM64 library is checked for required JNI exports, platform-only dependencies, and 16 KB ELF alignment. All required wave-monster sprite atlases are present.

This is an experimental first Android build. It has been compiled and its package checked, but has not been play-tested on an Android phone. Test touch interaction, audio, and saving/loading on a device before relying on the port for long sessions.

Original contributors and project documentation are retained in the main README.
