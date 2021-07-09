#pragma once

#include "WindowManagerX11.hh"
#include <jni.h>

namespace jwm {
    extern class AppX11 {
    public:
        void init(JNIEnv* jniEnv);
        void start();
        void terminate();

        WindowManagerX11& getWindowManager() {
            return wm;
        }

        JNIEnv* getJniEnv() {
            return _jniEnv;
        }

        float getScale();

        JNIEnv* _jniEnv;
        WindowManagerX11 wm;
    } app;
}