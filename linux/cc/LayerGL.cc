#include <algorithm>
#include <iostream>
#include <jni.h>
#include "impl/Library.hh"
#include "impl/RefCounted.hh"
#include "WindowX11.hh"
#include <GL/gl.h>
#include <limits>
#include "AppX11.hh"

namespace jwm {

class LayerGL: public RefCounted, public ILayer {
public:
    WindowX11* fWindow;
    GLXContext _context = nullptr;
    using glXSwapIntervalEXT_t = void (*)(Display*, GLXDrawable, int);   
    glXSwapIntervalEXT_t _glXSwapIntervalEXT;

    LayerGL() = default;
    ~LayerGL() = default;

    void attach(WindowX11* window) {
        fWindow = jwm::ref(window);
        assert(fWindow->_layer == nullptr);
        fWindow->_layer = this;
        if (_context == nullptr) {
            _context = glXCreateContext(jwm::app.getDisplay(),
                                        window->x11VisualInfo,
                                        nullptr,
                                        true);
                                
        }
        
        makeCurrentForced();

        _glXSwapIntervalEXT = reinterpret_cast<glXSwapIntervalEXT_t>(glXGetProcAddress(reinterpret_cast<const GLubyte*>("glXSwapIntervalEXT")));
        setVsyncMode(VSYNC_ENABLED);
    }

    void setVsyncMode(VSync v) override {

        if (_glXSwapIntervalEXT) {
            _glXSwapIntervalEXT(jwm::app.getDisplay(),
                                fWindow->_x11Window,
                                v);
        }
    }

    void resize(int width, int height) {
        glClearStencil(0);
        glClearColor(0, 0, 0, 255);
        glStencilMask(0xffffffff);
        glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        glViewport(0, 0, width, height);
    }

    void swapBuffers() {
        glXSwapBuffers(jwm::app.getDisplay(), fWindow->_x11Window);
    }

    void close() {
        glXDestroyContext(jwm::app.getDisplay(), _context);
        jwm::unref(&fWindow);
    }

    void makeCurrentForced() override {
        ILayer::makeCurrentForced();
        glXMakeCurrent(jwm::app.getDisplay(),
                       fWindow->_x11Window,
                       _context);
    }
};

} // namespace jwm

// JNI

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_LayerGL__1nMake
  (JNIEnv* env, jclass jclass) {
    jwm::LayerGL* instance = new jwm::LayerGL();
    return reinterpret_cast<jlong>(instance);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_LayerGL__1nAttach
  (JNIEnv* env, jobject obj, jobject windowObj) {
    jwm::LayerGL* instance = reinterpret_cast<jwm::LayerGL*>(jwm::classes::Native::fromJava(env, obj));
    jwm::WindowX11* window = reinterpret_cast<jwm::WindowX11*>(jwm::classes::Native::fromJava(env, windowObj));
    instance->attach(window);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_LayerGL__1nReconfigure
  (JNIEnv* env, jobject obj, jint width, jint height) {
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