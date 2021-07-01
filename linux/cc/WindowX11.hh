#pragma once

#include <jni.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include "Window.hh"
#include "WindowManagerX11.hh"

namespace jwm {
    class WindowX11: public jwm::Window {
    private:
        WindowManagerX11& mWindowManager;
    
    public:
        WindowX11(JNIEnv* env, WindowManagerX11& windowManager);
        ~WindowX11() override = default;

        bool init();
    };
}