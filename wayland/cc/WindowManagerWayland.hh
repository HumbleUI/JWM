#pragma once

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
#include <list>
#include "Output.hh"
#include <libdecor-0/libdecor.h>
#include <xkbcommon/xkbcommon.h>
#include <wayland-egl.h>
#include <EGL/egl.h> 

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

        WindowWayland* getWindowForNative(wl_surface* surface);

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

       
        static libdecor_interface _decorInterface;
        static void libdecorError(libdecor* context, enum libdecor_error error, const char* message);

        static wl_keyboard_listener _keyboardListener;
        static void keyboardKeymap(void* data, wl_keyboard* keyboard,
                        uint32_t format, int32_t fd, uint32_t size);
        static void keyboardEnter(void* data, wl_keyboard* keyboard,
                        uint32_t serial, wl_surface* surface, wl_array* keys);
        static void keyboardLeave(void* data, wl_keyboard* keyboard, uint32_t serial,
                        wl_surface* surface);
        static void keyboardKey(void* data, wl_keyboard* keyboard, uint32_t serial,
                        uint32_t time, uint32_t key, uint32_t state);
        static void keyboardModifiers(void* data, wl_keyboard* keyboard, uint32_t serial,
                        uint32_t modsDepressed, uint32_t modsLatched, uint32_t modLocked,
                        uint32_t group);
        static void keyboardRepeatInfo(void* data, wl_keyboard* keyboard, int32_t rate, int32_t delay);

        static wl_seat_listener _seatListener;

        static void seatCapabilities(void* data, wl_seat* seat, uint32_t capabilities);
        static void seatName(void* data, wl_seat* seat, const char* name);

        static wl_data_source_listener _sourceListener;
        static wl_data_offer_listener _offerListener;
        static wl_data_device_listener _deviceListener;

        ByteBuf getClipboardContents(const std::string& type);
        std::vector<std::string> getClipboardFormats();

        wl_display* display = nullptr;
        wl_registry* registry = nullptr;
        wl_shm* shm = nullptr;
        wl_compositor* compositor = nullptr;
        wl_data_device_manager* deviceManager = nullptr;
        // no multiseat?
        wl_seat* seat = nullptr;
        wl_pointer* pointer = nullptr;
        wl_keyboard* keyboard = nullptr;
        wl_data_device* dataDevice = nullptr;
        wl_data_source* currentSource = nullptr;
        uint32_t mouseSerial = -1;
        uint32_t keyboardSerial = -1;
        libdecor* decorCtx = nullptr;
        xkb_context* _xkbContext = nullptr;
        xkb_state* _xkbState = nullptr;
        EGLDisplay _eglDisplay = EGL_NO_DISPLAY;
        std::list<Output*> outputs;

        bool _runLoop;
        int notifyFD = -1;
        std::atomic_bool notifyBool{false};
        int lastMousePosX = 0;
        int lastMousePosY = 0;
        int mouseMask = 0;
        void mouseUpdate(WindowWayland* myWindow, uint32_t x, uint32_t y, uint32_t mask);

        std::map<wl_surface*, WindowWayland*> _nativeWindowToMy;
        std::map<std::string, ByteBuf> _myClipboardContents;
        std::list<std::string> _currentMimeTypes;


        wl_surface* cursorSurface;
        wl_surface* focusedSurface = nullptr;
        wl_surface* keyboardFocus = nullptr;
        // Is holding all cursors in memory a good idea?
        wl_cursor_image* _cursors[static_cast<int>(jwm::MouseCursor::COUNT)];


        std::mutex _taskQueueLock;
        std::condition_variable _taskQueueNotify;
        std::queue<std::function<void()>> _taskQueue;


        void setClipboardContents(std::map<std::string, ByteBuf>&& c);
    };
}
