#pragma once

#include <PlatformWin32.hh>
#include <Key.hh>
#include <jni.h>
#include <unordered_set>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <vector>

namespace jwm {

    class WindowManagerWin32 {
    public:
        bool init();
        int iteration();

    public:
        HWND getHelperWindow() const { return _hWndHelperWindow; }
        const std::unordered_map<int, Key> &getKeyTable() const { return _keyTable; }

    private:
        int _registerWindowClass();
        int _createHelperWindow();
        void _initKeyTable();
        void _registerWindow(class WindowWin32& window);
        void _unregisterWindow(class WindowWin32& window);

    private:
        friend class WindowWin32;

        std::unordered_map<HWND, class WindowWin32*> _windows;
        std::unordered_map<int, Key> _keyTable;

        HWND _hWndHelperWindow;
    };

}