#pragma once

#include <jni.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include "Window.hh"
#include "WindowManagerX11.hh"
#include "ILayer.hh"

namespace jwm {
    class WindowX11: public jwm::Window {
    public:
        WindowX11(JNIEnv* env, WindowManagerX11& windowManager);
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

        void setLayer(ILayer* layer) {
            _layer = layer;
        }

        /**
         * _NET_WM_SYNC_REQUEST (resize flicker fix) update request counter
         */
        struct {
            uint32_t lo = 0;
            uint32_t hi = 0;
            XID counter;
        } _xsyncRequestCounter;

        int _posX = 0;
        int _posY = 0;
        int _width = 0;
        int _height = 0;

        bool _isRedrawRequested = false;

        WindowManagerX11& _windowManager;
        ILayer* _layer = nullptr;
        ::Window _x11Window = 0;
        XIC _ic;

    };
}