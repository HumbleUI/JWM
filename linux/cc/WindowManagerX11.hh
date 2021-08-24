#pragma once

#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <map>
#include <memory>
#include <vector>
#include "Types.hh"
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace jwm {
    class WindowX11;
    class WindowManagerX11 {
    public:
        WindowManagerX11();

        void runLoop();
        void terminate();

        void registerWindow(WindowX11* window);
        void unregisterWindow(WindowX11* window);
        
        XVisualInfo* pickVisual();
        static int _xerrorhandler(Display* dsp, XErrorEvent* error);
        void _xi2IterateDevices();

        Display* getDisplay() const { return display; }
        ::Window getRootWindow() const;
        Screen* getScreen() const { return screen; }
        XVisualInfo* getVisualInfo() const { return x11VisualInfo; }
        XSetWindowAttributes& getSWA() { return x11SWA; }
        XIM getIM() const { return _im; }

        void enqueueTask(const std::function<void()>& task);

        void _processXEvent(XEvent& ev);
        void _processCallbacks();

        ByteBuf getClipboardContents(const std::string& type);
        std::vector<std::string> getClipboardFormats();

        Display* display = nullptr;
        Screen* screen;
        XVisualInfo* x11VisualInfo;
        XSetWindowAttributes x11SWA;
        bool _runLoop;

        std::map<::Window, WindowX11*> _nativeWindowToMy;
        std::map<std::string, ByteBuf> _myClipboardContents;

        /**
         * Input Manager
         */
        XIM _im;

        std::mutex _taskQueueLock;
        std::condition_variable _taskQueueNotify;
        std::queue<std::function<void()>> _taskQueue;

        struct XInput2 {
            int opcode;

            struct Device {
                struct ScrollValuator {
                    bool isHorizontal;
                    int number;
                    double increment;
                    double previousValue = 0;
                };
                std::vector<ScrollValuator> scroll;
            };
            std::map<int, Device> deviceById;
        };

        std::unique_ptr<XInput2> _xi2;

        struct Atoms {
            Atoms(Display* display): _display(display) {}

            // display definition here allows to reference Display* in DEFINE_ATOM
            Display* _display;

            #define DEFINE_ATOM(name) Atom name = XInternAtom(_display, #name, 0)
            
            // protocols
            // NOTE: WM_DELETE_WINDOW should be the first protocol because WindowX11 references to this variable
            const static int PROTOCOL_COUNT = 2;
            DEFINE_ATOM(WM_DELETE_WINDOW);
            DEFINE_ATOM(_NET_WM_SYNC_REQUEST);
            
            // other atoms
            DEFINE_ATOM(_NET_WM_STATE);
            DEFINE_ATOM(_NET_WM_NAME);
            DEFINE_ATOM(_NET_WM_STATE_MAXIMIZED_VERT);
            DEFINE_ATOM(_NET_WM_STATE_MAXIMIZED_HORZ);
            DEFINE_ATOM(_NET_WM_SYNC_REQUEST_COUNTER);
            DEFINE_ATOM(WM_PROTOCOLS);
            DEFINE_ATOM(UTF8_STRING);
            DEFINE_ATOM(CLIPBOARD);
            DEFINE_ATOM(JWM_CLIPBOARD);
            DEFINE_ATOM(INCR);
            DEFINE_ATOM(TARGETS);

            #undef DEFINE_ATOM
        } _atoms;
        Atoms& getAtoms() { return _atoms; }
        void setClipboardContents(std::map<std::string, ByteBuf>&& c);
    };
}