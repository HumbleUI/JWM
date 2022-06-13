# 0.4.3 - June 13, 2022

Added:

- Resizing mouse cursors #231 via @dzaima

Fixed:

- Catch exception in _eventListener.accept(e) to remove it from the queue
- macOS: fixed Window.setIcon
- macOS: race condition when requesting frame from non-UI thread

# 0.4.2 - April 21, 2022

Added:

- macOS: text/plain clipboard support #225 #51 via @bumfo @lgorence

Fixed:

- Set ContextClassLoader for App thread
- Check whether is closed before invoking native methods #229 via @dzaima

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