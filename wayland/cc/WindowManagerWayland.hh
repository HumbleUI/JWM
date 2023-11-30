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
#include "xdg_shell.h"

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
        void enqueueTask(const std::function<void()>& task);

        void registryHandleGlobal(void* data, wl_registry *registry,
                uint32_t name, const char* interface, uint32_t version);
        void registryHandleGlobalRemove(void* data, wl_registry *registry,
                uint32_t name);

       

        ByteBuf getClipboardContents(const std::string& type);
        std::vector<std::string> getClipboardFormats();

        wl_display* display = nullptr
        wl_registry* registry = nullptr
        wl_shm* shm = nullptr;
        zxdg_shell_v6* xdgShell = nullptr;
        wl_compositor* compositor = nullptr;
        wl_data_device_manager* deviceManager = nullptr;
        wl_seat* seat = nullptr;

        // XVisualInfo* x11VisualInfo;
        // XSetWindowAttributes x11SWA;
        bool _runLoop;
        int notifyFD = -1;
        std::atomic_bool notifyBool{false};
        int lastMousePosX = 0;
        int lastMousePosY = 0;
        void mouseUpdate(WindowWayland* myWindow);

        std::map<::Window, WindowWayland*> _nativeWindowToMy;
        std::map<std::string, ByteBuf> _myClipboardContents;


        wl_surface* cursorSurface;
        // Is holding all cursors in memory a good idea?
        wl_buffer _cursors[static_cast<int>(jwm::MouseCursor::COUNT)];


        std::mutex _taskQueueLock;
        std::condition_variable _taskQueueNotify;
        std::queue<std::function<void()>> _taskQueue;


        void setClipboardContents(std::map<std::string, ByteBuf>&& c);
    };
}
