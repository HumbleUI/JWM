#pragma once

#include <PlatformWin32.hh>
#include <Key.hh>
#include <jni.h>
#include <unordered_set>
#include <unordered_map>
#include <atomic>
#include <vector>
#include <mutex>

namespace jwm {

    class WindowManagerWin32 {
    public:
        bool init();
        int iteration();
        void timerUpdate();

    public:
        HWND getHelperWindow() const { return _hWndHelperWindow; }
        const std::unordered_map<int, Key> &getKeyTable() const { return _keyTable; }
        const std::unordered_set<int> &getKeyIgnoreList() const { return _keyIgnoreList; }

    private:
        int _registerWindowClass();
        int _createHelperWindow();
        void _initKeyTable();
        void _initKeyIgnoreList();
        void _registerWindow(class WindowWin32& window);
        void _unregisterWindow(class WindowWin32& window);
        void _dispatchFrameEvents();

    private:
        friend class WindowWin32;

        std::unordered_map<HWND, class WindowWin32*> _windows;
        std::unordered_map<int, Key> _keyTable;
        std::unordered_set<int> _keyIgnoreList;

        HWND _hWndHelperWindow = nullptr;

        mutable std::mutex _accessMutex;
    };

}