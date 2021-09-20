#pragma once

#include <PlatformWin32.hh>
#include <GL/gl.h>
#include <GL/wglext.h>
#include <mutex>

namespace jwm {

    class ContextWGL {
    public:
        bool init();
        bool finalize();

    public:
        PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
        PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
        PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB = nullptr;
        PFNWGLGETEXTENSIONSSTRINGEXTPROC wglGetExtensionsStringEXT = nullptr;
        PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = nullptr;

    private:
        bool _initialized = false;

        mutable std::mutex _accessMutex;
    };

}