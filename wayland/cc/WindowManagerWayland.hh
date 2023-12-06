#pragma once

#include <GL/glx.h>
#include <atomic>
#include <map>
#include <memory>
#include <vector>
#include "Types.hh"
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "MouseCursor.hh"
#include <wayland-client.h>
#include <wayland-cursor.h>
#include "xdg-shell/xdg-shell.h"

namespace jwm {
    class WindowWayland;
    class WindowManagerWayland {
    public:
        WindowManagerWayland();

        void runLoop();
        void terminate();

        void registerWindow(WindowWayland* window);
        void unregisterWindow(WindowWayland* window);
        
        // XVisualInfo* pickVisual();
        // static int _xerrorhandler(Display* dsp, XErrorEvent* error);
        // void _xi2IterateDevices();

        // XVisualInfo* getVisualInfo() const { return x11VisualInfo; }
        // XSetWindowAttributes& getSWA() { return x11SWA; }
        // XIM getIM() const { return _im; }
        /*
        int getX11VisualDepth() const {
            if (x11VisualInfo) {
                return x11VisualInfo->depth;
            }
            return DefaultDepth(display, 0);
        }
        Visual* getX11Visual() const {         
            if (x11VisualInfo) {
                return x11VisualInfo->visual;
            }
            return DefaultVisual(display, 0);
        }
        */
        void _processCallbacks();
        void notifyLoop();
        void enqueueTask(const std::function<void()>& task);

        static wl_registry_listener _registryListener;

        static void registryHandleGlobal(void* data, wl_registry *registry,
                uint32_t name, const char* interface, uint32_t version);
        static void registryHandleGlobalRemove(void* data, wl_registry *registry,
                uint32_t name);

        static wl_pointer_listener _pointerListener;

        static void pointerHandleEnter(void* data, wl_pointer *pointer, 
                uint32_t serial, wl_surface *surface, wl_fixed_t surface_x, wl_fixed_t surface_y);
        static void pointerHandleLeave(void* data, wl_pointer *pointer,
                uint32_t serial, wl_surface* surface);
        static void pointerHandleMotion(void* data, wl_pointer *pointer,
                uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y);
        static void pointerHandleButton(void* data, wl_pointer *pointer,
                uint32_t serial, uint32_t time, uint32_t button,
                uint32_t state);
        static void pointerHandleAxis(void* data, wl_pointer *pointer,
                uint32_t time, uint32_t axis, wl_fixed_t value);

        static xdg_wm_base_listener _xdgWmBaseListener;
        static void xdgWmBasePing(void* data, xdg_wm_base* base, uint32_t serial);
       

        ByteBuf getClipboardContents(const std::string& type);
        std::vector<std::string> getClipboardFormats();

        wl_display* display = nullptr;
        wl_registry* registry = nullptr;
        wl_shm* shm = nullptr;
        xdg_wm_base* xdgShell = nullptr;
        wl_compositor* compositor = nullptr;
        wl_data_device_manager* deviceManager = nullptr;
        wl_seat* seat = nullptr;
        wl_pointer* pointer = nullptr;


        // XVisualInfo* x11VisualInfo;
        // XSetWindowAttributes x11SWA;
        bool _runLoop;
        int notifyFD = -1;
        std::atomic_bool notifyBool{false};
        int lastMousePosX = 0;
        int lastMousePosY = 0;
        int mouseMask = 0;
        void mouseUpdate(WindowWayland* myWindow, uint32_t x, uint32_t y, uint32_t mask);

        std::map<wl_surface*, WindowWayland*> _nativeWindowToMy;
        std::map<std::string, ByteBuf> _myClipboardContents;


        wl_surface* cursorSurface;
        wl_surface* focusedSurface = nullptr;
        // Is holding all cursors in memory a good idea?
        wl_cursor_image* _cursors[static_cast<int>(jwm::MouseCursor::COUNT)];


        std::mutex _taskQueueLock;
        std::condition_variable _taskQueueNotify;
        std::queue<std::function<void()>> _taskQueue;


        void setClipboardContents(std::map<std::string, ByteBuf>&& c);
    };
}
