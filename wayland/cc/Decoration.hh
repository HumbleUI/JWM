#pragma once

#include <wayland-client.h>
#include <wayland-xdg-shell-client-protocol.h>
#include <wayland-xdg-decoration-unstable-v1-client-protocol.h>
#include <wayland-viewporter-client-protocol.h>
#include <functional>
#include "Buffer.hh"
#include <string>

#define DECORATION_WIDTH 10

#define DECORATION_HIDDEN_TOP_Y -(DECORATION_BOTTOM_HEIGHT)
#define DECORATION_TOP_X 0
#define DECORATION_TOP_Y -(DECORATION_TOP_HEIGHT)
#define DECORATION_TOP_WIDTH(window) window.getUnscaledWidth()
#define DECORATION_TOP_HEIGHT DECORATION_WIDTH * 3

#define DECORATION_HIDDEN_LEFT_Y DECORATION_HIDDEN_TOP_Y
#define DECORATION_HIDDEN_LEFT_HEIGHT(window) window.getUnscaledHeight() + DECORATION_WIDTH + DECORATION_WIDTH
#define DECORATION_LEFT_X -(DECORATION_WIDTH)
#define DECORATION_LEFT_Y -(DECORATION_TOP_HEIGHT)
#define DECORATION_LEFT_WIDTH DECORATION_WIDTH
#define DECORATION_LEFT_HEIGHT(window) window.getUnscaledHeight() + DECORATION_TOP_HEIGHT + DECORATION_WIDTH

#define DECORATION_HIDDEN_RIGHT_Y DECORATION_HIDDEN_LEFT_Y
#define DECORATION_HIDDEN_RIGHT_HEIGHT(window) DECORATION_HIDDEN_LEFT_HEIGHT(window)
#define DECORATION_RIGHT_X(window) window.getUnscaledWidth()
#define DECORATION_RIGHT_Y -(DECORATION_TOP_HEIGHT)
#define DECORATION_RIGHT_WIDTH DECORATION_WIDTH
#define DECORATION_RIGHT_HEIGHT(window) window.getUnscaledHeight() + DECORATION_TOP_HEIGHT + DECORATION_WIDTH

#define DECORATION_BOTTOM_X 0
#define DECORATION_BOTTOM_Y(window) window.getUnscaledHeight()
#define DECORATION_BOTTOM_WIDTH(window) window.getUnscaledWidth()
#define DECORATION_BOTTOM_HEIGHT DECORATION_WIDTH

#define DECORATION_CLOSE_X(window) window.getUnscaledWidth() - 10
#define DECORATION_CLOSE_Y -20
#define DECORATION_CLOSE_WIDTH 9
#define DECORATION_CLOSE_HEIGHT 9

#define DECORATION_MAX_X(window) (DECORATION_CLOSE_X(window)) - 10
#define DECORATION_MAX_Y -20
#define DECORATION_MAX_WIDTH 9
#define DECORATION_MAX_HEIGHT 9

#define DECORATION_MIN_X(window) (DECORATION_MAX_X(window)) - 10
#define DECORATION_MIN_Y -20
#define DECORATION_MIN_WIDTH 9
#define DECORATION_MIN_HEIGHT 9

namespace jwm {
    class WindowManagerWayland;
    class WindowWayland;
    struct DecorationPart {
        wl_surface* surface = nullptr;
        wl_subsurface* subsurface = nullptr;
        wp_viewport* viewport = nullptr;
    };
    enum DecorationFocus {
        DECORATION_FOCUS_MAIN,
        DECORATION_FOCUS_TOP,
        DECORATION_FOCUS_LEFT,
        DECORATION_FOCUS_RIGHT,
        DECORATION_FOCUS_BOTTOM,
        DECORATION_FOCUS_CLOSE_BUTTON,
        DECORATION_FOCUS_MAX_BUTTON,
        DECORATION_FOCUS_MIN_BUTTON
    };
    // Creation is mapping
    // Closing is unmapping
    class Decoration {
    public:
        Decoration() = delete;
        Decoration(WindowWayland& window);
        ~Decoration();

        WindowManagerWayland& _wm;
        WindowWayland& _window;

        Buffer* _decBuffer;
        Buffer* _closeBuffer;
        Buffer* _maxBuffer;
        Buffer* _minBuffer;

        DecorationPart _top;
        DecorationPart _left;
        DecorationPart _right;
        DecorationPart _bottom;

        DecorationPart _close;
        DecorationPart _max;
        DecorationPart _min;
        // May be null at runtime
        zxdg_toplevel_decoration_v1* _decoration = nullptr;

        xdg_surface* _xdgSurface = nullptr;
        xdg_toplevel* _xdgToplevel = nullptr;

        bool _serverSide = false;
        int _pendingWidth = 0;
        int _pendingHeight = 0;
        bool _oldActive = false;
        // : )
        bool _active = true;
        bool _oldMaximized = false;
        bool _maximized = false;
        bool _oldFullscreen = false;
        bool _fullscreen = false;
        bool _floating = true;
        bool _configured = false;
        // unmap and dispose
        void close();

        void _makePart(DecorationPart* part, Buffer* buf, bool opaque, int x, int y, int width, int height);
        void _resizeDecoration(DecorationPart* part, int x, int y, int width, int height);
        void _destroyDecoration(DecorationPart* part);

        void _adaptSize();
        void _destroyDecorations();
        void _showDecorations(bool hidden);

        static const char* proxyTag;
        static bool ownDecorationSurface(wl_surface* surface);
        static Decoration* getDecorationForSurface(wl_surface* surface, DecorationFocus* focus);

        std::string _title;

        void setTitle(const std::string& title);

        bool _isVisible = true;
        void setTitlebarVisible(bool isVisible);

        void getBorders(int& left, int& top, int& right, int& bottom);


    private:
        Decoration(Decoration&& other) = delete;
        Decoration& operator=(Decoration&& other) = delete;

        Decoration(Decoration& other) = delete;
        Decoration& operator=(Decoration& other) = delete;


                
    };
}
