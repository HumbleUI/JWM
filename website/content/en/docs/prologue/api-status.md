---
title: "API Status"
description: "Status."
lead: ""
date: 2020-11-16T13:59:39+01:01
lastmod: 2020-11-16T13:59:39+01:01
draft: false
images: []
menu:
  docs:
    parent: "prologue"
weight: 125
toc: true
---
### App

|                   | Windows | macOS | X11 |
|-------------------|---------|-------|-----|
| init              | ✅   | ✅   | ✅   |
| makeWindow        | ✅   | [#121](https://github.com/HumbleUI/JWM/issues/121)   | ✅   |
| start             | ✅   | ✅   | ✅   |
| getScreens        | ✅   | ✅   | ✅   |
| getPrimaryScreen  | ✅   | ✅   | ✅   |
| runOnUIThread     | ✅   | ✅   | [#113](https://github.com/HumbleUI/JWM/issues/113) |
| terminate         | ✅   | ✅   | ✅   |
| Show notification | ❌   | ❌   | ❌   |

### Theme

|                   | Windows | macOS | X11 |
|-------------------|---------|-------|-----|
| isHighContrast    | ✅      | ✅    | ➖  |
| isDark            | ✅      | ✅ | ➖ |
| isInverted        | [#161](https://github.com/HumbleUI/JWM/issues/161) | ✅ | ➖ |

### Window

|                     | Windows | macOS | X11 |
|---------------------|---------|-------|-----|
| setEventListener    | ✅   | ✅   | ✅   |
| setTextInputClient  | ✅   | ✅   | ✅   |
| setTextInputEnabled | ✅   | ❌   | ❌   |
| unmarkText          | ✅   | ❌   | ❌   |
| show                | ✅   | ✅   | ✅   |
| getWindowRect       | ✅   | ✅   | [#109](https://github.com/HumbleUI/JWM/issues/109) |
| getContentRect      | ✅   | ✅   | ✅   |
| setWindowPosition   | ✅   | ✅   | ✅   |
| setWindowSize       | ✅   | ✅   | [#109](https://github.com/HumbleUI/JWM/issues/109) |
| setContentSize      | ✅   | ✅   | ✅   |
| getScreen           | ✅   | ✅   | ✅   |
| requestFrame        | ✅   | ✅   | [#113](https://github.com/HumbleUI/JWM/issues/113) |
| close               | ✅   | [#107](https://github.com/HumbleUI/JWM/issues/107) | ✅   |
| minimize            | ✅   | ✅  |  ✅   |
| maximize            | ✅   | ✅  |  ✅   |
| restore             | ✅   | ✅  |  ✅   |
| setTitle            | ✅   | ✅   | ✅   |
| setIcon             | ✅   | ✅   | [#95](https://github.com/HumbleUI/JWM/issues/95)  |
| Set system cursor   | ✅   | ✅   | [#99](https://github.com/HumbleUI/JWM/issues/99)  |
| Customize titlebar  | [#75](https://github.com/HumbleUI/JWM/issues/75)  | [#75](https://github.com/HumbleUI/JWM/issues/75)  | [#75](https://github.com/HumbleUI/JWM/issues/75)  |
| focus               | ❌   | ✅   | ❌   |
| Get ZOrder          | ❌   | ✅   | ❌   |
| Set custom cursor   | ❌   | ❌   | ❌   |
| openFile            | ❌   | ❌   | ❌   |
| openFolder          | ❌   | ❌   | ❌   |
| Transparency        | ✅   | ❌   | ❌   |
| Toggle Fullscreen   | ❌   | ❌   | ❌   |
| setMinimumSize      | ❌   | ❌   | ❌   |
| setMaximumSize      | ❌   | ❌   | ❌   |
| setResizable        | ❌   | ❌   | ❌   |

### Events

|                         | Windows | macOS | X11 |
|-------------------------|---------|-------|-----|
| EventFrame              | ✅   | ✅   | ✅   |
| EventKey                | ✅   | ✅   | ✅   |
| EventMouseButton        | ✅   | ✅   | ✅   |
| EventMouseMove          | ✅   | ✅   | ✅   |
| EventMouseScroll        | [#115](https://github.com/HumbleUI/JWM/issues/115) | ✅   | [#115](https://github.com/HumbleUI/JWM/issues/115) |
| EventTextInput          | ✅   | [#105](https://github.com/HumbleUI/JWM/issues/105) | ✅   |
| EventTextInputMarked    | ✅   | ✅   | ✅   |
| EventWindowCloseRequest | ✅   | ✅   | ✅   |
| EventWindowMove         | ✅   | [#116](https://github.com/HumbleUI/JWM/issues/116) | ✅   |
| EventWindowResize       | ✅   | ✅   | ✅   |
| EventWindowMinimize     | ✅   | ✅   | [#96](https://github.com/HumbleUI/JWM/issues/96)   |
| EventWindowMaximize     | ✅   | ✅   | [#96](https://github.com/HumbleUI/JWM/issues/96)   |
| EventWindowRestore      | ✅   | ✅   | [#96](https://github.com/HumbleUI/JWM/issues/96)   |
| EventWindowVisible      | [#140](https://github.com/HumbleUI/JWM/issues/140) | [#140](https://github.com/HumbleUI/JWM/issues/140)   | [#140](https://github.com/HumbleUI/JWM/issues/140)   |
| EventWindowScreenChange | [#117](https://github.com/HumbleUI/JWM/issues/117) | [#117](https://github.com/HumbleUI/JWM/issues/117) | [#117](https://github.com/HumbleUI/JWM/issues/117) |
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
| workArea                | ✅      | ✅    | [#119](https://github.com/HumbleUI/JWM/issues/119) |
| colorSpace              | [#122](https://github.com/HumbleUI/JWM/issues/122) | [#122](https://github.com/HumbleUI/JWM/issues/122) | [#122](https://github.com/HumbleUI/JWM/issues/122) |

### Clipboard

|                | Windows | macOS | X11 |
|----------------|---------|-------|-----|
| set            | ✅      | [#51](https://github.com/HumbleUI/JWM/issues/51)   | ✅  |
| get            | ✅      | [#51](https://github.com/HumbleUI/JWM/issues/51)   | ✅  |
| getFormats     | ✅      | [#51](https://github.com/HumbleUI/JWM/issues/51)   | ✅  |
| clear          | ✅      | [#51](https://github.com/HumbleUI/JWM/issues/51)   | ✅  |
| registerFormat | ✅      | [#51](https://github.com/HumbleUI/JWM/issues/51)   | ✅  |

### Layers

|            | Windows | macOS | X11 |
|------------|---------|-------|-----|
| Raster     | ✅      | [#81](https://github.com/HumbleUI/JWM/issues/81)   | ✅  |
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
