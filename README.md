# Java Window Management library

<img src="./extras/jwm_logo.svg" height="400">

## Vision

JWM is a cross-platform Java window management and OS integration library.

JWM aims to expose an interface that abstracts over window creation, input handling and OS integration. It’s planned to support all the primary desktop platforms:

- Windows
- macOS
- X11
- Wayland

Primary goals:

- Native JVM API
- High-quality OS integration (indistinguashable from native apps)
- Plays well with (but does not require) [Skija](https://github.com/jetbrains/skija)

Motto: **“Electron for JVM, without Chrome and JS”**

## Alternatives

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

GLFW via LWJGL, SDL2:

- Game-oriented
- Bad window management
- No OS integration
- Create one window and go full-screen is the main use-case

Electron:

- Seems great, will use as inspiration

## Scope

TBD, but these are a good start:

- https://github.com/rust-windowing/winit/blob/master/FEATURES.md
- https://docs.google.com/spreadsheets/d/1fQfYrtVzWbthFb_8e9Q0ANVDt5oeQT2iAMcQlPi_YeU/edit?usp=sharing
- https://www.electronjs.org/docs

## Status

Proof of concept, do not use.

Repository URL:

```
https://packages.jetbrains.team/maven/p/jwm/maven
```

Platform    | groupId             | artifactId        | version
------------|---------------------|-------------------|----------
macOS Intel | `org.jetbrains.jwm` | `jwm-macos-x64`   | ![version](https://img.shields.io/badge/dynamic/xml?style=flat-square&label=latest&color=success&url=https%3A%2F%2Fpackages.jetbrains.team%2Fmaven%2Fp%2Fjwm%2Fmaven%2Forg%2Fjetbrains%2Fjwm%2Fjwm-macos-x64%2Fmaven-metadata.xml&query=//release)
macOS M1    | `org.jetbrains.jwm` | `jwm-macos-arm64` | ![version](https://img.shields.io/badge/dynamic/xml?style=flat-square&label=latest&color=success&url=https%3A%2F%2Fpackages.jetbrains.team%2Fmaven%2Fp%2Fjwm%2Fmaven%2Forg%2Fjetbrains%2Fjwm%2Fjwm-macos-arm64%2Fmaven-metadata.xml&query=//release)
Linux       | N/A                 | N/A               | N/A
Windows     | N/A                 | N/A               | N/A

## Developing

Prerequisites:

Git, CMake, Ninja, C++ compiler, JDK 11+, $JAVA_HOME, Python 3.

Build:

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