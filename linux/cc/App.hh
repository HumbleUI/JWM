#pragma once

#include "WindowManagerX11.hh"

namespace jwm {
    extern class App {
    public:
        void init();
        void start();

        WindowManagerX11& getWindowManager() {
            return wm;
        }

        WindowManagerX11 wm;
    } app;
}