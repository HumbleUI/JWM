#pragma once

#include <WindowManagerWin32.hh>
#include <ClipboardWin32.hh>
#include <D3D12/DX12Common.hh>
#include <ContextWGL.hh>
#include <ScreenWin32.hh>
#include <jni.h>

namespace jwm {

    class AppWin32 {
    public:
        void init(JNIEnv* jniEnv);
        int start();
        void terminate();
        bool isTerminateRequested() const;
        void enqueueCallback(jobject callback);
        const std::vector<ScreenWin32> &getScreens();
        LRESULT processEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);

    public:
        WindowManagerWin32& getWindowManager() { return _windowManager; }
        ClipboardWin32& getClipboard() { return _clipboard; }
        ContextWGL& getContextWGL() { return _wglContext; }
        DX12Common& getDx12Common() { return _dx12common; }
        JNIEnv* getJniEnv() const { return _jniEnv; }

    private:
        std::vector<ScreenWin32> _screens;
        WindowManagerWin32 _windowManager;
        ClipboardWin32 _clipboard;
        ContextWGL _wglContext;
        DX12Common _dx12common;
        JNIEnv* _jniEnv;
        bool _terminateRequested = false;

    public:
        static BOOL enumMonitorFunc(HMONITOR monitor, HDC dc, LPRECT rect, LPARAM data);
        static AppWin32& getInstance() { return gInstance; }
        static AppWin32 gInstance;
    };
}