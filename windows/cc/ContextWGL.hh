#pragma once

#include <PlatformWin32.hh>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/wglext.h>

namespace jwm {

    class ContextWGL {
    public:
        int init();
        int finalize();

        bool _initialized = false;
        PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
        PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
        PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB = nullptr;
        PFNWGLGETEXTENSIONSSTRINGEXTPROC wglGetExtensionsStringEXT = nullptr;
        PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = nullptr;
    };

}