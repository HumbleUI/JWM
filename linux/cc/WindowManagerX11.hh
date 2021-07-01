#pragma once

#include <X11/X.h>
#include <X11/Xlib.h>

namespace jwm {
    class WindowManagerX11 {
    private:
        Display* mDisplay = nullptr;
        Screen* mScreen;

        static int xerrorhandler(Display* dsp, XErrorEvent* error);
    public:
        WindowManagerX11();
    };
}