#pragma once

#include <jni.h>
#include <wayland-client.h>
#include "Window.hh"
#include "WindowManagerWayland.hh"
#include <ILayer.hh>
#include "ScreenInfo.hh"
#include "xdg-shell.hh"
#include <libdecor-0/libdecor.h>
namespace jwm {
    class WindowWayland: public jwm::Window {
    public:
        WindowWayland(JNIEnv* env, WindowManagerWayland& windowManager);
        ~WindowWayland() override;

        void getDecorations(int& left, int& top, int& right, int& bottom);
        void getContentPosition(int& posX, int& posY);
        void setVisible(bool isVisible);
        void close();
        bool init();
        void recreate();
        int getLeft();
        int getTop();
        int getWidth();
        int getHeight();
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
        void setLayer(ILayer* layer) {
            _layer = layer;
        }


        const ScreenInfo& getScreen();

        static void surfaceEnter(void* data, wl_surface* surface, wl_output* output);
        static void surfaceLeave(void* data, wl_surface* surface, wl_output* output);
        static void surfacePreferredBufferScale(void* data, wl_surface* surface, int factor);
        static void surfacePreferredBufferTransform(void* data, wl_surface* surface, uint32_t transform);

        static void xdgSurfaceConfigure(void* data, xdg_surface* surface, uint32_t serial);

        static void xdgToplevelConfigure(void* data, xdg_toplevel* toplevel, int width, int height, wl_array* states);
        static void xdgToplevelClose(void* data, xdg_toplevel* toplevel);
        static void xdgToplevelConfigureBounds(void* data, xdg_toplevel* toplevel, int width, int height);
        static void xdgToplevelWmCapabilities(void* data, xdg_toplevel* toplevel, wl_array* capabilities);

        void _adaptSize(int newWidth, int newHeight);


        int _posX = -1;
        int _posY = -1;
        int _width = -1;
        int _newWidth = -1;
        int _scale = 1;
        int _height = -1;
        int _newHeight = -1;
        int _WM_ADD = 1L;
        int _WM_REMOVE = 0L;
        bool _canMinimize = false;
        bool _canMaximize = false;
        bool _canFullscreen = false;
        bool _visible = false;

        bool _isRedrawRequested = false;

        WindowManagerWayland& _windowManager;
        ILayer* _layer = nullptr;
        wl_surface* _waylandWindow = nullptr;
        xdg_surface* xdgSurface = nullptr;
        xdg_toplevel* xdgToplevel = nullptr;
        libdecor_frame* _frame = nullptr;
    };
}
