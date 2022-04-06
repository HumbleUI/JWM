# 0.4.1 - April 6, 2022

Changed:

- Renamed Objective-C classes to avoid conflict with AWT

Fixed:

- X11: fix notifyLoop being called in parallel to main loop end via @dzaima
- X11: reset scroll distance after re-entering window #214 via @dzaima
- X11: correct window positions in EventWindowResize on maximize/restore #198
- X11: fix black screen after setVisible
- X11: Fixed mouse down on horizontal scroll (closes #217)

# 0.4.0 - February 16, 2022

Changed:

- Package pre-lomboked sources in -sources.jar #211
- [ BREAKING ] `App.init()` + `App.start()` -> `App.start(Runnable)`
- macOS: -XstartOnMainThread is no longer necessary #213 via @mworzala

Fixed:

- X11: Fixed clipboard #215 via @dzaima
- X11: Clear modifiers on EventWindowFocusOut #216 via @dzaima
- X11: Fixed EventFrame being issued twice per resize #212