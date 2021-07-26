#pragma once

#include <jni.h>
#include <X11/X.h>
#include <X11/Xlib.h>

namespace jwm {
    extern class AppX11 {
    public:
        void init(JNIEnv* jniEnv);
        void start();
        void terminate();

        Display* getDisplay();

        float getScale();

        bool _running = true;
    } app;
}