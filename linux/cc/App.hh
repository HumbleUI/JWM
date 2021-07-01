#pragma once

#include "WindowManagerX11.hh"
#include <jni.h>

namespace jwm {
    extern class App {
    public:
        void init(JNIEnv* jniEnv);
        void start();

        WindowManagerX11& getWindowManager() {
            return wm;
        }

        JNIEnv* getJniEnv() {
            return _jniEnv;
        }

        JNIEnv* _jniEnv;
        WindowManagerX11 wm;
    } app;
}