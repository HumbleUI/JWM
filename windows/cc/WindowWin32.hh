#pragma once
#include <Window.hh>
#include <PlatformWin32.hh>
#include <jni.h>

namespace jwm {
    class WindowWin32: public Window {
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

        void setLayer(class LayerGL* layer) { _layer = layer; }
        class LayerGL* getLayer() const { return _layer; }
        JNIEnv* getJNIEnv() const { return fEnv; }
        DWORD _getWindowStyle() const;
        DWORD _getWindowExStyle() const;

        class WindowManagerWin32& _windowManager;
        class LayerGL* _layer = nullptr;

        HWND _hWnd = NULL;
    };
}