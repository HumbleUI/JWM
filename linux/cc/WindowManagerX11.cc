#include "WindowManagerX11.hh"
#include <cstdio>

using namespace jwm;

int WindowManagerX11::xerrorhandler(Display* dsp, XErrorEvent* error) {
    char errorstring[0x100];
    XGetErrorText(dsp, error->error_code, errorstring, sizeof(errorstring));
    printf("X Error: %s\n", errorstring);
    return 0;
}

WindowManagerX11::WindowManagerX11() {
    mDisplay = XOpenDisplay(nullptr);
    XSetErrorHandler(xerrorhandler);
    mScreen = DefaultScreenOfDisplay(mDisplay);
}