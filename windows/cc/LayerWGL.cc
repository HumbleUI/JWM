#include <LayerWGL.hh>
#include <AppWin32.hh>
#include <WindowWin32.hh>
#include <WindowManagerWin32.hh>
#include <ContextWGL.hh>
#include <impl/Library.hh>
#include <jni.h>

void jwm::LayerWGL::attach(jwm::WindowWin32* window) {
    assert(!_windowWin32);

    AppWin32& app = AppWin32::getInstance();

    if (!window) {
        app.sendError("Passed null WindowWin32 object to attach");
        return;
    }

    if (window->testFlag(WindowWin32::Flag::HasAttachedLayer)) {
        app.sendError("Window already has attached layer. Cannot re-attach.");
        return;
    }

    if (window->testFlag(WindowWin32::Flag::RecreateForNextLayer)) {
        // HACK: if user creates dx12 layer, after that gl context
        // cannot be normally setup (don't know why, am I stupid?).
        // This hack allows to know about previous dx12 layer,
        // therefore we can recreate entire window (and its DC) under the hood.
        window->removeFlag(WindowWin32::Flag::RecreateForNextLayer);
        window->recreate();
    }

    _windowWin32 = jwm::ref(window);
    _windowWin32->setFlag(WindowWin32::Flag::HasAttachedLayer);

    // If have no rendering context for window, then create it here
    if (_hRC == nullptr) {
        WindowManagerWin32& winMan = app.getWindowManager();
        ContextWGL& contextWgl = app.getContextWGL();

        // Init context, if it is not initialized yet
        if (!contextWgl.init()) {
            app.sendError("Failed to initialize WGL globals");
            return;
        }

        // Get window device context
        _hDC = GetDC(_windowWin32->getHWnd());

        const int pixelAttribs[] = {
            WGL_SWAP_METHOD_EXT, WGL_SWAP_EXCHANGE_EXT,
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

        bool status = contextWgl.wglChoosePixelFormatARB(_hDC, pixelAttribs, nullptr, 1, &pixelFormatID, &numFormats);

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
        _hRC = contextWgl.wglCreateContextAttribsARB(_hDC, nullptr, contextAttribs);

        if (!_hRC) {
            app.sendError("Failed to create rendering context");
            _releaseInternal();
            return;
        }

        // Listen for window events
        _callbackID = _windowWin32->addEventListener([this](WindowWin32::Event event){
            switch (event) {
                case WindowWin32::Event::SwitchContext:
                    makeCurrent();
                    break;
                case WindowWin32::Event::SwapBuffers:
                    swapBuffers();
                    break;
                case WindowWin32::Event::EnableVsync:
                    vsync(true);
                    break;
                case WindowWin32::Event::DisableVsync:
                    vsync(false);
                    break;
                default:
                    return;
            }
        });
    }

    // Make context current (since we running in separate thread it won't be changed)
    if (!wglMakeCurrent(_hDC, _hRC)) {
        app.sendError("Failed to make rendering context current");
        _releaseInternal();
        return;
    }

    // Force v-sync (maybe in the future we will add some options)
    vsync(true);
}

void jwm::LayerWGL::resize(int width, int height) {
    glClearStencil(0);
    glClearColor(0, 0, 0, 255);
    glStencilMask(0xffffffff);
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, width, height);
}

void jwm::LayerWGL::swapBuffers() {
    assert(_hDC);
    wglSwapLayerBuffers(_hDC, WGL_SWAP_MAIN_PLANE);
}

void jwm::LayerWGL::makeCurrent() {
    assert(_hDC);
    assert(_hRC);

    HGLRC currentRC = wglGetCurrentContext();

    if (currentRC != _hRC)
        wglMakeCurrent(_hDC, _hRC);
}

void jwm::LayerWGL::close() {
    _releaseInternal();
}

void jwm::LayerWGL::vsync(bool enable) {
    AppWin32& app = AppWin32::getInstance();
    ContextWGL& contextWgl = app.getContextWGL();

    int interval = enable?
        static_cast<int>(Vsync::EnableAdaptive):
        static_cast<int>(Vsync::Disable);

    if (contextWgl.wglSwapIntervalEXT)
        contextWgl.wglSwapIntervalEXT(interval);
}


void jwm::LayerWGL::_releaseInternal() {
    if (_hRC) {
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(_hRC);
        _hRC = nullptr;
    }

    if (_hDC) {
        ReleaseDC(_windowWin32->getHWnd(), _hDC);
        _hDC = nullptr;
    }

    if (_windowWin32) {
        _windowWin32->removeFlag(WindowWin32::Flag::HasAttachedLayer);
        _windowWin32->removeEventListener(_callbackID);
        jwm::unref(&_windowWin32);
    }
}

// JNI

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_LayerGL__1nMake
        (JNIEnv* env, jclass jclass) {
    jwm::LayerWGL* instance = new jwm::LayerWGL();
    return reinterpret_cast<jlong>(instance);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_LayerGL__1nAttach
        (JNIEnv* env, jobject obj, jobject windowObj) {
    jwm::LayerWGL* instance = reinterpret_cast<jwm::LayerWGL*>(jwm::classes::Native::fromJava(env, obj));
    jwm::WindowWin32* window = reinterpret_cast<jwm::WindowWin32*>(jwm::classes::Native::fromJava(env, windowObj));
    instance->attach(window);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_LayerGL__1nReconfigure
        (JNIEnv* env, jobject obj, jint width, jint height) {
    // todo: what to do here?
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_LayerGL__1nResize
        (JNIEnv* env, jobject obj, jint width, jint height) {
    jwm::LayerWGL* instance = reinterpret_cast<jwm::LayerWGL*>(jwm::classes::Native::fromJava(env, obj));
    instance->resize(width, height);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_LayerGL__1nSwapBuffers
        (JNIEnv* env, jobject obj) {
    //jwm::LayerWGL* instance = reinterpret_cast<jwm::LayerWGL*>(jwm::classes::Native::fromJava(env, obj));
    //instance->swapBuffers();
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_LayerGL__1nClose
        (JNIEnv* env, jobject obj) {
    jwm::LayerWGL* instance = reinterpret_cast<jwm::LayerWGL*>(jwm::classes::Native::fromJava(env, obj));
    instance->close();
}