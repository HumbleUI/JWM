#pragma once

#include <jni.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include "Window.hh"
#include "WindowManagerX11.hh"

namespace jwm {
    class WindowX11: public jwm::Window {
    public:
        WindowX11(JNIEnv* env, WindowManagerX11& windowManager);
        ~WindowX11() override;

        void getPosition(int& posX, int& posY);
        void show();
        bool init();
        int getLeft();
        int getTop();
        int getWidth();
        int getHeight();

        float getScale();

        WindowManagerX11& _windowManager;
        ::Window _x11Window = 0;

    };
}