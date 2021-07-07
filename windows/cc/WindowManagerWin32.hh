#pragma once

#include <PlatformWin32.hh>
#include <Key.hh>
#include <unordered_set>
#include <unordered_map>
#include <mutex>
#include <atomic>

namespace jwm {

    class WindowManagerWin32 {
    public:
        bool init();
        int runMainLoop();
        void requestTerminate();
        void requestFrame(class WindowWin32* window);

        HWND getHelperWindow() const { return _hWndHelperWindow; }
        const std::unordered_map<int, Key> &getKeyTable() const { return _keyTable; }

        int _registerWindowClass();
        int _createHelperWindow();
        void _initKeyTable();
        void _registerWindow(class WindowWin32& window);
        void _unregisterWindow(class WindowWin32& window);

        std::unordered_set<class WindowWin32*> _windows;
        std::unordered_set<class WindowWin32*> _frameRequests;
        std::unordered_map<int, Key> _keyTable;
        std::atomic_bool _terminateRequested{false};

        HWND _hWndHelperWindow;
    };

}