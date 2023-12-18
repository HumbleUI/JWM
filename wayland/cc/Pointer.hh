#pragma once

#include <wayland-client.h>

namespace jwm {
    class WindowManagerWayland;
    class WindowWayland;
    class Pointer {
    public:
        Pointer(wl_pointer* pointer, jwm::WindowManagerWayland* wm);
        ~Pointer();

        wl_pointer* _pointer;
        wl_pointer* getPointer() const {
            return _pointer;
        }

        uint32_t _serial = 0;
        uint32_t getSerial() {
            return _serial;
        }

        wl_surface* _surface;
        wl_surface* getSurface() const {
            return _surface;
        }

        WindowWayland* _focusedSurface;
        WindowWayland* getFocusedSurface() const {
            return _focusedSurface;
        }

        uint32_t _lastMouseX;
        uint32_t _lastMouseY;
        float _dX = 0.0;
        float _dY = 0.0;

        int _mouseMask = 0;
        jwm::WindowManagerWayland& _wm;

        static wl_pointer_listener _pointerListener;

        void mouseUpdate(uint32_t x, uint32_t y, uint32_t mask);
        void mouseUpdateUnscaled(uint32_t x, uint32_t y, uint32_t mask);

        void updateHotspot(int x, int y);

    };
}
