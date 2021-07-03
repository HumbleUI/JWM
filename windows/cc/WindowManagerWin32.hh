#pragma once

#include <PlatformWin32.hh>
#include <unordered_set>
#include <mutex>
#include <atomic>

namespace jwm {

    class WindowManagerWin32 {
    public:
        int init();
        int runMainLoop();
        void requestTerminate();
        void sendError(const char* what);

        HWND getHelperWindow() const { return _hWndHelperWindow; }

        int _registerWindowClass();
        int _createHelperWindow();
        void _registerWindow(class WindowWin32& window);
        void _unregisterWindow(class WindowWin32& window);

        std::unordered_set<class WindowWin32*> _windows;
        std::atomic_bool _terminateRequested{false};

        HWND _hWndHelperWindow;

        mutable std::mutex _accessMutex;
    };

}