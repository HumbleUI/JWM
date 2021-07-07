#pragma once

#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <map>

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

        Display* getDisplay() const { return display; }
        Screen* getScreen() const { return screen; }
        XVisualInfo* getVisualInfo() const { return x11VisualInfo; }
        XSetWindowAttributes& getSWA() { return x11SWA; }
        XIM getIM() const { return _im; }

        Display* display = nullptr;
        Screen* screen;
        XVisualInfo* x11VisualInfo;
        XSetWindowAttributes x11SWA;
        bool _runLoop;

        std::map<::Window, WindowX11*> _nativeWindowToMy;

        /**
         * Input Manager
         */
        XIM _im;

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
            DEFINE_ATOM(_NET_WM_SYNC_REQUEST_COUNTER);
            DEFINE_ATOM(WM_PROTOCOLS);

            #undef DEFINE_ATOM
        } _atoms;
        Atoms& getAtoms() { return _atoms; }
    };
}