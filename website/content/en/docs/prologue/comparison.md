---
title: "Comparison to other UI/window libraries"
description: "Comparison to other UI/window libraries"
lead: ""
date: 2020-11-16T13:59:39+01:01
lastmod: 2020-11-16T13:59:39+01:01
draft: false
images: []
menu:
  docs:
    parent: "prologue"
weight: 115
toc: true
---


Traditionally, Java UIs struggled with high-quality OS integration. JWM plans to bring it to modern standards by implementing those integrations from scratch in modern low-level OS-native APIs. Prior art:

## AWT:

- Bullet-proof, works everywhere
- Event loop is two-threaded (lags in UI)
- Dated font management, color management, dpi management
- No vsync

## JavaFX:

- Fixed threading model
- Performance is [sometimes great, sometimes terrible](https://github.com/tonsky/java-graphics-benchmark/)
- Even more limited fonts/color/graphics API
- VSync is weird in multi-monitor case
- No real extensibility

## Winit

- [https://github.com/rust-windowing/winit](https://github.com/rust-windowing/winit)
- Tried at JetBrains
- Complicated event loop model (tries to unify desktop + web + mobile)
- [https://github.com/rust-windowing/winit/blob/master/FEATURES.md](https://github.com/rust-windowing/winit/blob/master/FEATURES.md)

## GLFW via LWJGL, SDL2:

- Game-oriented
- Bad window management
- No OS integration
- Create one window and go full-screen is the main use-case

## Electron:

- Seems great, will use as inspiration
- [https://www.electronjs.org/docs](https://www.electronjs.org/docs)
