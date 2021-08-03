#pragma once
#include <PlatformWin32.hh>
#include <impl/RefCounted.hh>
#include <vector>
#include <jni.h>

namespace jwm {

    class LayerRaster: public RefCounted {
    public:
        enum class Vsync {
            Disable = 0,
            Enable = 1
        };

    public:
        void attach(class WindowWin32* window);
        void resize(int width, int height);
        void swapBuffers();
        void close();
        void vsync(bool enable);

    public:
        void* getPixelsPtr() const;
        int getRowBytes() const;

    private:
        void _releaseInternal();

    private:
        std::vector<uint8_t> _bitmapMemory;

        class WindowWin32* _windowWin32 = nullptr;
        BITMAPINFO* _bitmapInfo = nullptr;
        HDC _hDC = nullptr;

        int _width = 0;
        int _height = 0;
        int _callbackID = -1;
    };

}