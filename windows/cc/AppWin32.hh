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
        void sendError(const char* what);
        void enqueueUIThreadCallback(jobject callback);

        WindowManagerWin32& getWindowManager() { return _windowManager; }
        ContextWGL& getContextWGL() { return _wglContext; }
        JNIEnv* getJniEnv() const { return _jniEnv; }

        std::atomic_bool _terminateRequested{false};
        std::vector<jobject> _uiThreadCallbacks;
        WindowManagerWin32 _windowManager;
        ContextWGL _wglContext;
        JNIEnv* _jniEnv;

        static AppWin32& getInstance() { return gInstance; }
        static AppWin32 gInstance;
    };
}