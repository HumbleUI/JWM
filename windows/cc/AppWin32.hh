#pragma once

#include <WindowManagerWin32.hh>
#include <ContextWGL.hh>
#include <ScreenWin32.hh>
#include <jni.h>

namespace jwm {

    class AppWin32 {
    public:
        void init(JNIEnv* jniEnv);
        int start();
        void terminate();
        void sendError(const char* what);
        void enqueueUIThreadCallback(jobject callback);
        const std::vector<ScreenWin32> &getScreens();

    public:
        WindowManagerWin32& getWindowManager() { return _windowManager; }
        ContextWGL& getContextWGL() { return _wglContext; }
        JNIEnv* getJniEnv() const { return _jniEnv; }

    private:
        std::atomic_bool _terminateRequested{false};
        std::vector<jobject> _uiThreadCallbacks;
        std::vector<ScreenWin32> _screens;
        WindowManagerWin32 _windowManager;
        ContextWGL _wglContext;
        JNIEnv* _jniEnv;

    public:
        static BOOL enumMonitorFunc(HMONITOR monitor, HDC dc, LPRECT rect, LPARAM data);
        static AppWin32& getInstance() { return gInstance; }
        static AppWin32 gInstance;
    };
}