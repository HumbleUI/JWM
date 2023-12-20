#pragma once

#include <jni.h>
#include <wayland-client.h>
#include "Window.hh"
#include "WindowManagerWayland.hh"
#include "ILayerWayland.hh"
#include "ScreenInfo.hh"
#include <libdecor-0/libdecor.h>
#include <string>
namespace jwm {
    class WindowWayland: public jwm::Window {
    public:
        WindowWayland(JNIEnv* env, WindowManagerWayland& windowManager);
        ~WindowWayland() override;

        void getDecorations(int& left, int& top, int& right, int& bottom);
        void getContentPosition(int& posX, int& posY);
        void setVisible(bool isVisible);
        bool resize(int width, int height);
        void close();
        void hide();
        bool init();
        void show();
        int getLeft();
        int getTop();
        int getWidth();
        int getUnscaledWidth();
        int getHeight();
        int getUnscaledHeight();
        float getScale();
        void requestRedraw() {
            _isRedrawRequested = true;
            _windowManager.notifyLoop();
        }
        void unsetRedrawRequest() {
            _isRedrawRequested = false;
        }
        bool isRedrawRequested() {
            return _isRedrawRequested;
        }
        void setTitle(const std::string& title);
        void setTitlebarVisible(bool isVisible);

        void maximize();
        void minimize();
        void restore();

        void setFullScreen(bool isFullScreen);
        bool isFullScreen();

        void setCursor(jwm::MouseCursor cursor);
        void setLayer(ILayerWayland* layer);

        wl_cursor* _getCursorFor(jwm::MouseCursor cursor);
        ScreenInfo getScreen();

        static void surfaceEnter(void* data, wl_surface* surface, wl_output* output);
        static void surfaceLeave(void* data, wl_surface* surface, wl_output* output);
        static void surfacePreferredBufferScale(void* data, wl_surface* surface, int factor);
        static void surfacePreferredBufferTransform(void* data, wl_surface* surface, uint32_t transform);

        static void decorFrameConfigure(libdecor_frame* frame, libdecor_configuration* config, void* userData);
        static void decorFrameClose(libdecor_frame* frame, void* userData);
        static void decorFrameCommit(libdecor_frame* frame, void* userData);
        static void decorFrameDismissPopup(libdecor_frame* frame, const char* seatName, void* userData);

        void _adaptSize(int newWidth, int newHeight);


        int _posX = -1;
        int _posY = -1;
        int _width = -1;
        int _newWidth = -1;
        int _scale = 1;
        int _oldScale = 1;
        int _height = -1;
        int _newHeight = -1;
        int _WM_ADD = 1L;
        int _WM_REMOVE = 0L;
        int _floatingWidth = 400;
        int _floatingHeight = 400;
        bool _canMinimize = false;
        bool _canMaximize = false;
        bool _canFullscreen = false;
        bool _visible = false;
        bool _configured = false;
        bool _active = false;
        bool _maximized = false;
        bool _fullscreen = false;
        bool _floating = false;
        bool _isRedrawRequested = false;
        std::string _title;
        bool _titlebarVisible = true;

        WindowManagerWayland& _windowManager;
        ILayerWayland* _layer = nullptr;
        wl_surface* _waylandWindow = nullptr;
        libdecor_frame* _frame = nullptr;
        Output* _output = nullptr;
        wl_cursor_theme* theme = nullptr;

        static wl_surface_listener _surfaceListener;

        static libdecor_frame_interface _libdecorFrameInterface;

        static wl_callback_listener _frameCallback;

        static const char* _windowTag;

    };
}
