# 0.4.0 - February 16, 2022

Changed:

- Package pre-lomboked sources in -sources.jar #211
- [ BREAKING ] `App.init()` + `App.start()` -> `App.start(Runnable)`
- macOS: -XstartOnMainThread is no longer necessary #213 via @mworzala

Fixed:

- X11: Fixed clipboard #215 via @dzaima
- X11: Clear modifiers on EventWindowFocusOut #216 via @dzaima
- X11: Fixed EventFrame being issued twice per resize #212