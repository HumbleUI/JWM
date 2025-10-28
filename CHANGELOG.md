# 0.4.20 - Oct 28, 2025

- Update Skija compatibility to 0.123.0

# 0.4.19 - Apr 11, 2025

- macOS: fix build #296 via @SergeevPavel, @Alex2772

# 0.4.18 - Jul 9, 2024

- macOS: Stable screen ids
- macOS: Do not stop CVDisplayLink, improves frame pacing

# 0.4.17 - Jan 16, 2024

- X11: add KeyLocation, additional keys via @dzaima
- X11: fix i3 workspace switching #283 via @bulby
- macOS: Fix fullscreen

# 0.4.16 - Dec 1, 2023

Added:

- macOS: Window::bringToFront, ::isFront (#269, thx @Quezion)
- macOS: LayerMetal::GetMetalLayer (#275, thx @smallshen)
- X11: add held modifier info on mouse events when window isn't focused (#274, thx @dzaima)

Fixed:

- X11: Use XQueryPointer mouse position instead of XI_Motion fields (#272, thx @dzaima)
- X11: fix getScreenCoordinates (#218, #276, thx @TheDrawingCoder-Gamer)

# 0.4.15 - Apr 3, 2023

Added:

- Windows: Window::bringToFront & ::isFront #266 via @Quezion

# 0.4.14 - Mar 31, 2023

Added:

- TextInputClient::getSelectedRange, ::getMarkedRange and ::getSubstring #136

Fixed:

- macOS: Missed clicks when tap to click is enabled #267

# 0.4.13 - Jan 9, 2023

Added:

- Support fullscreen on X11 #259 by @jakeisnt

# 0.4.12 - Dec 16, 2022

Fixed:

- NSMutableDictionary allocation

# 0.4.11 - Dec 11, 2022

Added:

- macOS: EventTouchStart, EventTouchMove, EventTouchCancel, EventTouchEnd #249 #251 #254 via @shaunlebron
- macOS: Window::lockMouseCursor #253 via @shaunlebron

# 0.4.10 - Sep 27, 2022

Added:

- macOS: Window::setFullScreen, ::isFullScreen, EventWindowFullScreenEnter, EventWindowFullScreenExit #130
- macOS: Window::hideMouseCursorUntilMoved

# 0.4.9 - Sep 24, 2022

Changed:

- X11: Fixed mouse position in EventMouseButton/EventMouseScroll #241 #242 #243 #244 thx @navaratine @dzaima

# 0.4.8 - Sep 14, 2022

Changed:

- macOS: setFramebufferOnly:NO on Metal backend
- linux: avoid crash on empty X11 resources

# 0.4.7 - Aug 31, 2022

Changed:

- Mouse position is reported in EventMouseButton/EventMouseScroll
- Window::requestFrame now checks if window was already closed

# 0.4.6 - Aug 16, 2022

Changed:

- Window::requestFrame can be called from any thread #137

# 0.4.5 - Aug 13, 2022

Added:

- macOS: App::openSymbolsPalette
- X11: Window::setTitleBarVisible #235 via @mgxm

Fixed:

- X11: Fixed mouse scroll amount #236
- macOS: Fix cursor resetting to arrow #234 via @LuisThiamNye

# 0.4.4 - June 23, 2022

Added:

- macOS: Implement window focus events via @LuisThiamNye

Fixed:

- macOS: Fixed requestFrame when window is not visible

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