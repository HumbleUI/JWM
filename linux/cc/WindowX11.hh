#pragma once

#include <jni.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/glx.h>
#include "Window.hh"
#include "ILayer.hh"

namespace jwm {
    class WindowX11: public jwm::Window {
    public:
        WindowX11(JNIEnv* env);
        ~WindowX11() override;

        void getPosition(int& posX, int& posY);
        void show();
        void close();
        bool init();
        int getLeft();
        int getTop();
        int getWidth();
        int getHeight();
        float getScale();
        void move(int left, int top);
        void resize(int width, int height);
        void requestRedraw() {
            _isRedrawRequested = true;
        }
        void unsetRedrawRequest() {
            _isRedrawRequested = false;
        }
        bool isRedrawRequested() {
            return _isRedrawRequested;
        }

        XIC getIC() const {
            return _ic;
        }

        void runLoop();

        bool _runLoop = false;

        XVisualInfo* pickVisual();

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

        /**
         * _NET_WM_SYNC_REQUEST (resize flicker fix) update request counter
         */
        struct {
            uint32_t lo = 0;
            uint32_t hi = 0;
            XID counter;
            XID extended_counter;
        } _xsyncRequestCounter;

        int _width = 0;
        int _height = 0;

        bool _isRedrawRequested = false;

        Screen* screen;
        XVisualInfo* x11VisualInfo;
        XSetWindowAttributes x11SWA;
        ILayer* _layer = nullptr;
        ::Window _x11Window = 0;
        XIC _ic;

    };
}