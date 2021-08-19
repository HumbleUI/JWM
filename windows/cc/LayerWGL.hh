#pragma once
#include <PlatformWin32.hh>
#include <impl/RefCounted.hh>

namespace jwm {

    class LayerWGL: public RefCounted {
    public:
        enum class Vsync {
            Disable = 0,
            Enable = 1,
            EnableAdaptive = -1
        };

        static const int GL_MAJOR_MIN = 4;
        static const int GL_MINOR_MIN = 5;

    public:
        void attach(class WindowWin32* window);
        void resize(int width, int height);
        void reconfigure();
        void swapBuffers();
        void makeCurrent();
        void close();
        void vsync(bool enable);
        void requestSwap();

    private:
        void _releaseInternal();

    private:
        class WindowWin32* _windowWin32 = nullptr;
        HDC _hDC = nullptr;
        HGLRC _hRC = nullptr;
        int _callbackID = -1;
    };

}