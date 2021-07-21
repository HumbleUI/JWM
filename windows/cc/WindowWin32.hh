#pragma once
#include <Window.hh>
#include <PlatformWin32.hh>
#include <functional>
#include <utility>
#include <bitset>
#include <vector>
#include <jni.h>

namespace jwm {
    class WindowWin32 final: public Window {
    public:
        enum class Event {
            SwitchContext,
            SwapBuffers,
            EnableVsync,
            DisableVsync
        };

        enum class Flag: size_t {
            EnterSizeMove = 1,
            RequestFrame = 2,
            Max = 3
        };

        using Callback = std::function<void(Event)>;
        static const int BUTTON_DOWN = 0x8000;

    public:
        explicit WindowWin32(JNIEnv* env, class WindowManagerWin32& windowManagerWin32);
        ~WindowWin32() override;
        bool init();
        void show();
        void getPosition(int& left, int& top) const;
        void getSize(int& width, int& height) const;
        int getLeft() const;
        int getTop() const;
        int getWidth() const;
        int getHeight() const;
        float getScale() const;
        void move(int left, int top);
        void resize(int width, int height);
        void close();
        LRESULT processEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);

    public:
        int addEventListener(Callback callback);
        void removeEventListener(int callbackID);
        void notifyEvent(Event event);
        void setFlag(Flag flag) { _flags.set(static_cast<size_t>(flag), true); }
        void removeFlag(Flag flag) { _flags.set(static_cast<size_t>(flag), false); }
        bool getFlag(Flag flag) { return _flags.test(static_cast<size_t>(flag)); }
        JNIEnv* getJNIEnv() const { return fEnv; }
        HWND getHWnd() const { return _hWnd; }

    private:
        DWORD _getWindowStyle() const;
        DWORD _getWindowExStyle() const;
        int _getModifiers() const;
        int _getMouseButtons() const;
        int _getNextCallbackID();
        void _setFrameTimer();
        void _killFrameTimer();

    private:
        friend class WindowManagerWin32;

        std::vector<std::pair<int, Callback>> _onEventListeners;
        std::bitset<static_cast<size_t>(Flag::Max)> _flags;

        class WindowManagerWin32& _windowManager;

        HWND _hWnd = nullptr;
        int _nextCallbackID = 0;
    };
}