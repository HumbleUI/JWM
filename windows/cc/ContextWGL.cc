#include <ContextWGL.hh>
#include <AppWin32.hh>
#include <WindowManagerWin32.hh>

int jwm::ContextWGL::init() {
    if (_initialized)
        return JWM_TRUE;

    AppWin32& app = AppWin32::getInstance();
    WindowManagerWin32& winMan = app.getWindowManager();
    HWND helperWindow = winMan.getHelperWindow();

    // Create render context for helper window
    // in order to load wgl extension function to create moder window
    // Useful link https://mariuszbartosik.com/opengl-4-x-initialization-in-windows-without-a-framework/

    HDC helperWindowDC = GetDC(helperWindow);

    PIXELFORMATDESCRIPTOR helperWindowPFD;
    ZeroMemory(&helperWindowPFD, sizeof(helperWindowPFD));
    helperWindowPFD.nSize = sizeof(helperWindowPFD);
    helperWindowPFD.nVersion = 1;
    helperWindowPFD.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    helperWindowPFD.iPixelType = PFD_TYPE_RGBA;
    helperWindowPFD.cColorBits = 32;
    helperWindowPFD.cAlphaBits = 8;
    helperWindowPFD.cDepthBits = 24;

    int pixelFormatID = ChoosePixelFormat(helperWindowDC, &helperWindowPFD);

    if (!pixelFormatID) {
        winMan.sendError("Failed to choose pixel format for helper window");
        return JWM_FALSE;
    }

    if (!SetPixelFormat(helperWindowDC, pixelFormatID, &helperWindowPFD)) {
        winMan.sendError("Failed to set pixel format for helper window");
        return JWM_FALSE;
    }

    HGLRC helperWindowRC = wglCreateContext(helperWindowDC);

    if (!helperWindowRC) {
        winMan.sendError("Failed to create rendering context for helper window");
        return JWM_FALSE;
    }

    // Used to release helper window temporary rendering context
    auto releaseHelperWindowResources = [=](){
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(helperWindowRC);
        ReleaseDC(helperWindow, helperWindowDC);
    };

    if (!wglMakeCurrent(helperWindowDC, helperWindowRC)) {
        winMan.sendError("Failed to make helper window context current");
        releaseHelperWindowResources();
        return JWM_FALSE;
    }

    // Load functions now

    wglChoosePixelFormatARB = reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>
            (wglGetProcAddress("wglChoosePixelFormatARB"));
    wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>
            (wglGetProcAddress("wglCreateContextAttribsARB"));
    wglGetExtensionsStringARB = reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGARBPROC>
            (wglGetProcAddress("wglGetExtensionsStringARB"));
    wglGetExtensionsStringEXT = reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGEXTPROC>
            (wglGetProcAddress("wglGetExtensionsStringEXT"));
    wglSwapIntervalEXT = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>
            (wglGetProcAddress("wglSwapIntervalEXT"));

    releaseHelperWindowResources();

    return _initialized = JWM_TRUE;
}

int jwm::ContextWGL::finalize() {
    if (_initialized)
        _initialized = JWM_FALSE;

    return JWM_TRUE;
}
