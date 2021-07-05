#pragma once

#include <PlatformWin32.hh>
#include <GL/gl.h>
#include <GL/wglext.h>

namespace jwm {

    class ContextWGL {
    public:
        bool init();
        bool finalize();

        bool _initialized = false;

        PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
        PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
        PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB = nullptr;
        PFNWGLGETEXTENSIONSSTRINGEXTPROC wglGetExtensionsStringEXT = nullptr;
        PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = nullptr;
    };

}