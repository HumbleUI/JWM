#include <algorithm>
#include <iostream>
#include <jni.h>
#include "impl/Library.hh"
#include "impl/RefCounted.hh"
#include "WindowWayland.hh"
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GL/gl.h>
#include <wayland-egl.h>
#include "ILayerWayland.hh"

namespace jwm {

    class LayerGL: public RefCounted, public ILayerWayland {
    public:
        WindowWayland* fWindow;
        wl_egl_window* _eglWindow = nullptr;
        EGLContext _context = nullptr;
        EGLDisplay _display = nullptr;
        EGLSurface _surface = nullptr;
        EGLConfig _config = nullptr;

        LayerGL() = default;
        virtual ~LayerGL() = default;

        void attach(WindowWayland* window) {
            eglBindAPI(EGL_OPENGL_API);
            fWindow = jwm::ref(window);
            bool shouldReopen = false;
            if (window->_layer) {
              // HACK: close window and reopen
              window->close();
              shouldReopen = window->_visible;
            }
            fWindow->setLayer(this);
            if (shouldReopen)
              window->show();
            if (_display == nullptr) {
              _display = eglGetPlatformDisplay(EGL_PLATFORM_WAYLAND_KHR, window->_windowManager.display, nullptr);

              eglInitialize(_display, nullptr, nullptr);

            }
            if (_context == nullptr) {
                EGLint attrList[] = {
                  EGL_ALPHA_SIZE, 8,
                  EGL_BLUE_SIZE, 8,
                  EGL_GREEN_SIZE, 8,
                  EGL_RED_SIZE, 8,
                  EGL_NONE
                };
                EGLint numConfig;
                eglChooseConfig(_display, attrList, &_config, 1, &numConfig);
                // :troll:
                _context = eglCreateContext(_display,
                                            _config,
                                            EGL_NO_CONTEXT,
                                            nullptr);
            }
            
            makeCurrentForced();

        }

        void setVsyncMode(VSync v) override {
          // vsync? what vsync?
        }

        void resize(int width, int height) override {
            glClearStencil(0);
            glClearColor(0, 0, 0, 255);
            glStencilMask(0xffffffff);
            glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

            // ???
            glViewport(0, 0, width, height);
            // God is dead if _eglWindow is null 
            if (_eglWindow)
              wl_egl_window_resize(_eglWindow, width, height, 0, 0);
        }

        void swapBuffers() {
            eglSwapBuffers(_display, _surface);
        }

        void close() override {
            eglDestroyContext(_display, _context);
            jwm::unref(&fWindow);
        }

        void makeCurrentForced() override {
            ILayer::makeCurrentForced();
            eglMakeCurrent(_display,
                          _surface,
                          _surface,
                          _context);
        }
        void attachBuffer() override {
          if (fWindow && fWindow->_waylandWindow) {
            if (!_eglWindow) {
              _eglWindow = wl_egl_window_create(fWindow->_waylandWindow, fWindow->getWidth(), fWindow->getHeight());
              _surface = eglCreatePlatformWindowSurface(_display, _config, _eglWindow, nullptr);
            }
          }
        }
    };

} // namespace jwm

// JNI

extern "C" JNIEXPORT jlong JNICALL Java_io_github_humbleui_jwm_LayerGL__1nMake
  (JNIEnv* env, jclass jclass) {
    jwm::LayerGL* instance = new jwm::LayerGL();
    return reinterpret_cast<jlong>(instance);
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_LayerGL__1nAttach
  (JNIEnv* env, jobject obj, jobject windowObj) {
    try {
        jwm::LayerGL* instance = reinterpret_cast<jwm::LayerGL*>(jwm::classes::Native::fromJava(env, obj));
        jwm::WindowWayland* window = reinterpret_cast<jwm::WindowWayland*>(jwm::classes::Native::fromJava(env, windowObj));
        instance->attach(window);
    } catch (const std::exception& e) {
        jwm::classes::Throwable::throwLayerNotSupportedException(env, "Failed to init OpenGL");
    }
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_LayerGL__1nReconfigure
  (JNIEnv* env, jobject obj, jint width, jint height) {
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_LayerGL__1nResize
  (JNIEnv* env, jobject obj, jint width, jint height) {
    jwm::LayerGL* instance = reinterpret_cast<jwm::LayerGL*>(jwm::classes::Native::fromJava(env, obj));
    instance->resize(width, height);
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_LayerGL__1nMakeCurrent
  (JNIEnv* env, jobject obj) {
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_LayerGL__1nSwapBuffers
  (JNIEnv* env, jobject obj) {
    jwm::LayerGL* instance = reinterpret_cast<jwm::LayerGL*>(jwm::classes::Native::fromJava(env, obj));
    instance->swapBuffers();
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_LayerGL__1nClose
  (JNIEnv* env, jobject obj) {
    jwm::LayerGL* instance = reinterpret_cast<jwm::LayerGL*>(jwm::classes::Native::fromJava(env, obj));
    instance->close();
}
