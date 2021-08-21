# Java Window Management library

<p align="center">
  <img src="./extras/jwm_logo.svg" height="400">
</p>

## Vision

JWM is a cross-platform Java window management and OS integration library.

JWM aims to expose an interface that abstracts over window creation, input handling and OS integration. It’s planned to support all the primary desktop platforms:

- Windows
- macOS
- X11
- Wayland

Primary goals:

- Native JVM API
- High-quality OS integration (indistinguishable from native apps)
- Plays well with (but does not require) [Skija](https://github.com/jetbrains/skija)

Motto: **“Electron for JVM, without Chrome and JS”**

## Status

Alpha. Expect API breakages.

### App

|                   | Windows | macOS | X11 |
|-------------------|---------|-------|-----|
| init              | ✅   | ✅   | ✅   |
| makeWindow        | ✅   | [#121](https://github.com/jetbrains/jwm/issues/121)   | ✅   |
| start             | ✅   | ✅   | ✅   |
| getScreens        | ✅   | ✅   | ✅   |
| getPrimaryScreen  | ✅   | ✅   | ✅   |
| runOnUIThread     | [#113](https://github.com/jetbrains/jwm/issues/113) | ✅   | [#113](https://github.com/jetbrains/jwm/issues/113) |
| terminate         | ✅   | ✅   | ✅   |
| Show notification | ❌   | ❌   | ❌   |
| System Theme      | ❌   | ❌   | ❌   |

### Window

|                     | Windows | macOS | X11 |
|---------------------|---------|-------|-----|
| setEventListener    | ✅   | ✅   | ✅   |
| setTextInputClient  | ✅   | ✅   | ✅   |
| setTextInputEnabled | ✅   | ❌   | ❌   |
| unmarkText          | ✅   | ❌   | ❌   |
| show                | ✅   | ✅   | ✅   |
| getWindowRect       | ✅   | ✅   | [#109](https://github.com/jetbrains/jwm/issues/109) |
| getContentRect      | ✅   | ✅   | ✅   |
| setWindowPosition   | ✅   | ✅   | ✅   |
| setWindowSize       | ✅   | ✅   | [#109](https://github.com/jetbrains/jwm/issues/109) |
| setContentSize      | ✅   | ✅   | ✅   |
| getScreen           | ✅   | ✅   | [#103](https://github.com/jetbrains/jwm/issues/103) |
| requestFrame        | [#113](https://github.com/jetbrains/jwm/issues/113) | ✅   | [#113](https://github.com/jetbrains/jwm/issues/113) |
| close               | ✅   | [#107](https://github.com/jetbrains/jwm/issues/107) | ✅   |
| minimize            | [#96](https://github.com/jetbrains/jwm/issues/96)  | [#96](https://github.com/jetbrains/jwm/issues/96)  | [#96](https://github.com/jetbrains/jwm/issues/96)  |
| maximize            | [#96](https://github.com/jetbrains/jwm/issues/96)  | [#96](https://github.com/jetbrains/jwm/issues/96)  | [#96](https://github.com/jetbrains/jwm/issues/96)  |
| restore             | [#96](https://github.com/jetbrains/jwm/issues/96)  | [#96](https://github.com/jetbrains/jwm/issues/96)  | [#96](https://github.com/jetbrains/jwm/issues/96)  |
| setTitle            | ✅  | ✅  | [#94](https://github.com/jetbrains/jwm/issues/94)  |
| setIcon             | [#95](https://github.com/jetbrains/jwm/issues/95)  | [#95](https://github.com/jetbrains/jwm/issues/95)  | [#95](https://github.com/jetbrains/jwm/issues/95)  |
| Set system cursor       | [#99](https://github.com/jetbrains/jwm/issues/99)  | [#99](https://github.com/jetbrains/jwm/issues/99)  | [#99](https://github.com/jetbrains/jwm/issues/99)  |
| Remove decorations  | [#75](https://github.com/jetbrains/jwm/issues/75)  | [#75](https://github.com/jetbrains/jwm/issues/75)  | [#75](https://github.com/jetbrains/jwm/issues/75)  |
| Set custom cursor   | ❌   | ❌   | ❌   |
| openFile            | ❌   | ❌   | ❌   |
| openFolder          | ❌   | ❌   | ❌   |
| Transparency        | ❌   | ❌   | ❌   |
| Toggle Fullscreen   | ❌   | ❌   | ❌   |

### Events

|                         | Windows | macOS | X11 |
|-------------------------|---------|-------|-----|
| EventFrame              | ✅   | ✅   | ✅   |
| EventKey                | [#30](https://github.com/jetbrains/jwm/issues/30)  | [#30](https://github.com/jetbrains/jwm/issues/30) [#87](https://github.com/jetbrains/jwm/issues/87) | [#30](https://github.com/jetbrains/jwm/issues/30)  |
| EventMouseButton        | ✅   | ✅   | ✅   |
| EventMouseMove          | ✅   | ✅   | ✅   |
| EventMouseScroll        | [#115](https://github.com/jetbrains/jwm/issues/115) | ✅   | [#115](https://github.com/jetbrains/jwm/issues/115) |
| EventTextInput          | ✅   | [#105](https://github.com/jetbrains/jwm/issues/105) | ✅   |
| EventTextInputMarked    | ✅   | ✅   | ✅   |
| EventWindowCloseRequest | ✅   | ✅   | ✅   |
| EventWindowMove         | ✅   | [#116](https://github.com/jetbrains/jwm/issues/116) | ✅   |
| EventWindowResize       | ✅   | ✅   | ✅   |
| EventWindowScreenChange | [#117](https://github.com/jetbrains/jwm/issues/117) | [#117](https://github.com/jetbrains/jwm/issues/117) | [#117](https://github.com/jetbrains/jwm/issues/117) |
| Drag & Drop             | ❌   | ❌   | ❌   |
| Touch events            | ❌   | ❌   | ❌   |
| Theme Changed           | ❌   | ❌   | ❌   |


### Screen

|                         | Windows | macOS | X11 |
|-------------------------|---------|-------|-----|
| id                      | ✅      | ✅    | ✅  |
| isPrimary               | ✅      | ✅    | ✅  |
| bounds                  | ✅      | ✅    | ✅  |
| scale                   | ✅      | ✅    | ✅  |
| workArea                | [#119](https://github.com/jetbrains/jwm/issues/119) | ✅ | [#119](https://github.com/jetbrains/jwm/issues/119) |
| colorSpace              | [#122](https://github.com/jetbrains/jwm/issues/122) | [#122](https://github.com/jetbrains/jwm/issues/122) | [#122](https://github.com/jetbrains/jwm/issues/122) |

### Clipboard

|                | Windows | macOS | X11 |
|----------------|---------|-------|-----|
| set            | ✅      | [#51](https://github.com/jetbrains/jwm/issues/51)   | ✅  |
| get            | ✅      | [#51](https://github.com/jetbrains/jwm/issues/51)   | ✅  |
| getFormats     | ✅      | [#51](https://github.com/jetbrains/jwm/issues/51)   | ✅  |
| clear          | ✅      | [#51](https://github.com/jetbrains/jwm/issues/51)   | ✅  |
| registerFormat | ✅      | [#51](https://github.com/jetbrains/jwm/issues/51)   | ✅  |

### Layers

|            | Windows | macOS | X11 |
|------------|---------|-------|-----|
| Raster     | ✅      | [#81](https://github.com/jetbrains/jwm/issues/81)   | ✅  |
| OpenGL     | ✅      | ✅    | ✅  |
| DirectX 11 | ❌      | ➖    | ➖  |
| DirectX 12 | ✅      | ➖    | ➖  |
| Metal      | ➖      | ✅    | ➖  |
| Vulkan     | ❌      | ➖    | ❌  |

### Packaging

|                | Windows | macOS | X11 |
|----------------|---------|-------|-----|
| Run on GraalVM | ❌      | ❌    | ❌  |
| App package    | ❌      | ❌    | ❌  |

## Prior art

Traditionally, Java UIs struggled with high-quality OS integration. JWM plans to bring it to modern standards by implementing those integrations from scratch in modern low-level OS-native APIs. Prior art:

AWT:

- Bullet-proof, works everywhere
- Event loop is two-threaded (lags in UI)
- Dated font management, color management, dpi management
- No vsync

JavaFX:

- Fixed threading model
- Performance is [sometimes great, sometimes terrible](https://github.com/tonsky/java-graphics-benchmark/)
- Even more limited fonts/color/graphics API
- VSync is weird in multi-monitor case
- No real extensibility

[Winit](https://github.com/rust-windowing/winit):

- Tried at JetBrains
- Complicated event loop model (tries to unify desktop + web + mobile)
- https://github.com/rust-windowing/winit/blob/master/FEATURES.md

GLFW via LWJGL, SDL2:

- Game-oriented
- Bad window management
- No OS integration
- Create one window and go full-screen is the main use-case

Electron:

- Seems great, will use as inspiration
- https://www.electronjs.org/docs

## Using

Repository URL:

```
https://packages.jetbrains.team/maven/p/jwm/maven
```

Artifacts:

Platform    | groupId             | artifactId        | version
------------|---------------------|-------------------|----------
macOS Intel | `org.jetbrains.jwm` | `jwm-macos-x64`   | ![version](https://img.shields.io/badge/dynamic/xml?style=flat-square&label=latest&color=success&url=https%3A%2F%2Fpackages.jetbrains.team%2Fmaven%2Fp%2Fjwm%2Fmaven%2Forg%2Fjetbrains%2Fjwm%2Fjwm-macos-x64%2Fmaven-metadata.xml&query=//release)
macOS M1    | `org.jetbrains.jwm` | `jwm-macos-arm64` | ![version](https://img.shields.io/badge/dynamic/xml?style=flat-square&label=latest&color=success&url=https%3A%2F%2Fpackages.jetbrains.team%2Fmaven%2Fp%2Fjwm%2Fmaven%2Forg%2Fjetbrains%2Fjwm%2Fjwm-macos-arm64%2Fmaven-metadata.xml&query=//release)
Linux       | `org.jetbrains.jwm` | `jwm-linux-x64`   | ![version](https://img.shields.io/badge/dynamic/xml?style=flat-square&label=latest&color=success&url=https%3A%2F%2Fpackages.jetbrains.team%2Fmaven%2Fp%2Fjwm%2Fmaven%2Forg%2Fjetbrains%2Fjwm%2Fjwm-linux-x64%2Fmaven-metadata.xml&query=//release)
Windows     | `org.jetbrains.jwm` | `jwm-windows-x64` | ![version](https://img.shields.io/badge/dynamic/xml?style=flat-square&label=latest&color=success&url=https%3A%2F%2Fpackages.jetbrains.team%2Fmaven%2Fp%2Fjwm%2Fmaven%2Forg%2Fjetbrains%2Fjwm%2Fjwm-windows-x64%2Fmaven-metadata.xml&query=//release)

## Building from source

Prerequisites:

- Shared: Git, CMake, Ninja, C++ compiler, JDK 11+, $JAVA_HOME, Python 3
- Windows 10: Microsoft Visual C++ (MSVC), x64 Native Tools Command Prompt for VS
- Ubuntu 20.04: `libxcomposite-dev libxrandr-dev libgl1-mesa-dev libxi-dev`

Run:

```
./script/build.py
```

Run examples:

```
./script/run.py
```

Run examples without building (use version from the table above):

```
./script/run.py --jwm-version <version>
```

# Authors

- [Nikita Prokopov](https://github.com/tonsky)
- [Egor Orachyov](https://github.com/EgorOrachyov)
- [Alexey Titov](https://github.com/Alex2772)
- [Pavel Sergeev](https://github.com/SergeevPavel)

PRs & issue reports are welcome!