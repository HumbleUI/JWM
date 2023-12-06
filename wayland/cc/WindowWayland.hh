#pragma once

#include <jni.h>
#include <wayland-client.h>
#include "Window.hh"
#include "WindowManagerWayland.hh"
#include "ILayerWayland.hh"
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
        void show();
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
        void setLayer(ILayerWayland* layer) {
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

        static void outputGeometry(void* data, wl_output* output, int x, int y, int physWidth, int physHeight,
                int subpixelOrient, const char* make, const char* model, int transform);
        static void outputMode(void* data, wl_output* output, uint32_t flags, int width, int height, int refresh);
        static void outputDone(void* data, wl_output* output);
        // YEAH THAT'S WHAT I'VE BEEN WAITING FOR
        static void outputScale(void* data, wl_output* output, int factor);
        static void outputName(void* data, wl_output* output, const char* name);
        static void outputDescription(void* data, wl_output* output, const char* desc);

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
        ILayerWayland* _layer = nullptr;
        wl_surface* _waylandWindow = nullptr;
        xdg_surface* xdgSurface = nullptr;
        xdg_toplevel* xdgToplevel = nullptr;
        libdecor_frame* _frame = nullptr;
    };
}
