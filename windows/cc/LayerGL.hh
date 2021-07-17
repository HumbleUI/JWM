#include <PlatformWin32.hh>
#include <impl/RefCounted.hh>

namespace jwm {

    class LayerGL: public RefCounted {
    public:
        static const int GL_MAJOR_MIN = 4;
        static const int GL_MINOR_MIN = 5;

    public:
        void attach(class WindowWin32* window);
        void resize(int width, int height);
        void swapBuffers();
        void makeCurrent();
        void close();

    private:
        void _releaseInternal();

    private:
        class WindowWin32* _windowWin32 = nullptr;
        HDC _hDC = NULL;
        HGLRC _hRC = NULL;
    };

}