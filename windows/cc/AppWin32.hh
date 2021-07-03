#pragma once

#include <WindowManagerWin32.hh>
#include <ContextWGL.hh>
#include <jni.h>

namespace jwm {

    class AppWin32 {
    public:
        void init(JNIEnv* jniEnv);
        int start();
        void terminate();

        WindowManagerWin32& getWindowManager() { return _windowManager; }
        ContextWGL& getContextWGL() { return _wglContext; }
        JNIEnv* getJniEnv() const { return _jniEnv; }

        WindowManagerWin32 _windowManager;
        ContextWGL _wglContext;
        JNIEnv* _jniEnv;

        static AppWin32& getInstance() { return gInstance; }
        static AppWin32 gInstance;
    };
}