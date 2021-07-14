#include <LayerGL.hh>
#include <AppWin32.hh>
#include <WindowWin32.hh>
#include <WindowManagerWin32.hh>
#include <ContextWGL.hh>
#include <impl/Library.hh>
#include <jni.h>

void jwm::LayerGL::attach(jwm::WindowWin32* window) {
    assert(!_windowWin32);

    AppWin32& app = AppWin32::getInstance();
    WindowManagerWin32& winMan = app.getWindowManager();
    ContextWGL& contextWGL = app.getContextWGL();

    if (!window) {
        app.sendError("Passed null WindowWin32 object to attach");
        return;
    }

    _windowWin32 = jwm::ref(window);

    // If have no rendering context for window, then create it here
    if (_hRC == nullptr) {
        // Init context, if it is not initialized yet
        if (!contextWGL.init()) {
            app.sendError("Failed to initialize WGL globals");
            return;
        }

        // Get window device context
        _hDC = GetDC(_windowWin32->_hWnd);

        //  WGL_SWAP_METHOD_EXT
        //        If the color buffer has a back buffer, then this indicates how
        //        they are swapped. If it is set to WGL_SWAP_EXCHANGE_EXT then
        //        swapping exchanges the front and back buffer contents; if it is
        //        set to WGL_SWAP_COPY_EXT then swapping copies the back buffer
        //        contents to the front buffer; if it is set to
        //        WGL_SWAP_UNDEFINED_EXT then the back buffer contents are copied
        //        to the front buffer but the back buffer contents are undefined
        //        after the operation. The <iLayerPlane> parameter is ignored if
        //        this attribute is specified.

        const int pixelAttribs[] = {
            WGL_SWAP_METHOD_EXT, WGL_SWAP_COPY_EXT,
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
            WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
            WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
            WGL_COLOR_BITS_ARB, 32,
            WGL_ALPHA_BITS_ARB, 8,
            WGL_DEPTH_BITS_ARB, 24,
            WGL_STENCIL_BITS_ARB, 8,
            WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
            WGL_SAMPLES_ARB, 4,
            0
        };

        int pixelFormatID;
        UINT numFormats;

        bool status = contextWGL.wglChoosePixelFormatARB(_hDC, pixelAttribs, NULL, 1, &pixelFormatID, &numFormats);

        if (!status || numFormats == 0) {
            app.sendError("Failed to chose pixel format");
            _releaseInternal();
            return;
        }

        PIXELFORMATDESCRIPTOR PFD;
        DescribePixelFormat(_hDC, pixelFormatID, sizeof(PFD), &PFD);

        if (!SetPixelFormat(_hDC, pixelFormatID, &PFD)) {
            app.sendError("Failed to set selected pixel format");
            _releaseInternal();
            return;
        }

        int  contextAttribs[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, GL_MAJOR_MIN,
            WGL_CONTEXT_MINOR_VERSION_ARB, GL_MINOR_MIN,
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0
        };

        // Shared context? Maybe use this feature future
        _hRC = contextWGL.wglCreateContextAttribsARB(_hDC, nullptr, contextAttribs);

        if (!_hRC) {
            app.sendError("Failed to create rendering context");
            _releaseInternal();
            return;
        }

        _windowWin32->setLayer(this);
    }

    if (!wglMakeCurrent(_hDC, _hRC)) {
        app.sendError("Failed to make rendering context current");
        _releaseInternal();
        return;
    }

    if (contextWGL.wglSwapIntervalEXT)
        // Force v-sync for now
        contextWGL.wglSwapIntervalEXT(1);
}

void jwm::LayerGL::resize(int width, int height) {
    glClearStencil(0);
    glClearColor(0, 0, 0, 255);
    glStencilMask(0xffffffff);
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, width, height);
}

void jwm::LayerGL::swapBuffers() {
    assert(_hDC);
    SwapBuffers(_hDC);
}

void jwm::LayerGL::makeCurrent() {
    assert(_hDC);
    assert(_hRC);
    wglMakeCurrent(_hDC, _hRC);
}

void jwm::LayerGL::close() {
    _releaseInternal();
}

void jwm::LayerGL::_releaseInternal() {
    if (_hRC) {
        wglDeleteContext(_hRC);
        _hRC = nullptr;
    }

    if (_hDC) {
        ReleaseDC(_windowWin32->_hWnd, _hDC);
        _hDC = nullptr;
    }

    if (_windowWin32) {
        _windowWin32->setLayer(nullptr);
        jwm::unref(&_windowWin32);
    }
}

// JNI

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_LayerGL__1nMake
        (JNIEnv* env, jclass jclass) {
    jwm::LayerGL* instance = new jwm::LayerGL();
    return reinterpret_cast<jlong>(instance);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_LayerGL__1nAttach
        (JNIEnv* env, jobject obj, jobject windowObj) {
    jwm::LayerGL* instance = reinterpret_cast<jwm::LayerGL*>(jwm::classes::Native::fromJava(env, obj));
    jwm::WindowWin32* window = reinterpret_cast<jwm::WindowWin32*>(jwm::classes::Native::fromJava(env, windowObj));
    instance->attach(window);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_LayerGL__1nReconfigure
        (JNIEnv* env, jobject obj, jint width, jint height) {
    // todo: what to do here?
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_LayerGL__1nResize
        (JNIEnv* env, jobject obj, jint width, jint height) {
    jwm::LayerGL* instance = reinterpret_cast<jwm::LayerGL*>(jwm::classes::Native::fromJava(env, obj));
    instance->resize(width, height);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_LayerGL__1nSwapBuffers
        (JNIEnv* env, jobject obj) {
    jwm::LayerGL* instance = reinterpret_cast<jwm::LayerGL*>(jwm::classes::Native::fromJava(env, obj));
    instance->swapBuffers();
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_LayerGL__1nClose
        (JNIEnv* env, jobject obj) {
    jwm::LayerGL* instance = reinterpret_cast<jwm::LayerGL*>(jwm::classes::Native::fromJava(env, obj));
    instance->close();
}