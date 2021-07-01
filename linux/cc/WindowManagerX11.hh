#pragma once

#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/glx.h>

namespace jwm {
    class WindowManagerX11 {
    public:
        WindowManagerX11();

        void runLoop();

        XVisualInfo* pickVisual();
        static int _xerrorhandler(Display* dsp, XErrorEvent* error);

        Display* display = nullptr;
        Screen* screen;
        XVisualInfo* x11VisualInfo;
        XSetWindowAttributes x11SWA;

        /**
         * Input Manager
         */
        XIM im;

        Display* getDisplay() const { return display; }
        Screen* getScreen() const { return screen; }
        XVisualInfo* getVisualInfo() const { return x11VisualInfo; }
        XSetWindowAttributes& getSWA() { return x11SWA; }
    };
}