#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

#include <algorithm>
#import  <Cocoa/Cocoa.h>
#include <iostream>
#include <jni.h>
#include "impl/Library.hh"
#include "impl/RefCounted.hh"
#include "Log.hh"
#include <OpenGL/gl.h>
#include "WindowMac.hh"

namespace jwm {

class LayerGL: public RefCounted {
public:
    WindowMac* fWindow;
    NSOpenGLContext* fGLContext;
    NSOpenGLPixelFormat* fPixelFormat;

    LayerGL() = default;
    ~LayerGL() = default;

    void attach(JNIEnv* env, WindowMac* window) {
        fWindow = jwm::ref(window);

        // set up pixel format
        constexpr int kMaxAttributes = 19;
        NSOpenGLPixelFormatAttribute attributes[kMaxAttributes];
        int numAttributes = 0;
        attributes[numAttributes++] = NSOpenGLPFAAccelerated;
        attributes[numAttributes++] = NSOpenGLPFAClosestPolicy;
        attributes[numAttributes++] = NSOpenGLPFADoubleBuffer;
        attributes[numAttributes++] = NSOpenGLPFAOpenGLProfile;
        attributes[numAttributes++] = NSOpenGLProfileVersion3_2Core;
        attributes[numAttributes++] = NSOpenGLPFAColorSize;
        attributes[numAttributes++] = 24;
        attributes[numAttributes++] = NSOpenGLPFAAlphaSize;
        attributes[numAttributes++] = 8;
        attributes[numAttributes++] = NSOpenGLPFADepthSize;
        attributes[numAttributes++] = 0;
        attributes[numAttributes++] = NSOpenGLPFAStencilSize;
        attributes[numAttributes++] = 8;
        // if (fMSAASampleCount > 1) {
        //     attributes[numAttributes++] = NSOpenGLPFAMultisample;
        //     attributes[numAttributes++] = NSOpenGLPFASampleBuffers;
        //     attributes[numAttributes++] = 1;
        //     attributes[numAttributes++] = NSOpenGLPFASamples;
        //     attributes[numAttributes++] = fMSAASampleCount;
        // } else {
            attributes[numAttributes++] = NSOpenGLPFASampleBuffers;
            attributes[numAttributes++] = 0;
        // }
        attributes[numAttributes++] = 0;

        fPixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
        if (nil == fPixelFormat) {
            JWM_LOG("Failed to initialize NSOpenGLPixelFormat");
            classes::Throwable::throwLayerNotSupportedException(env, "Failed to initialize NSOpenGLPixelFormat");
            return;
        }

        // create context
        fGLContext = [[NSOpenGLContext alloc] initWithFormat:fPixelFormat shareContext:nil];
        if (nil == fGLContext) {
            [fPixelFormat release];
            fPixelFormat = nil;
            JWM_LOG("Failed to initialize NSOpenGLContext");
            classes::Throwable::throwLayerNotSupportedException(env, "Failed to initialize NSOpenGLContext");
            return;
        }

        NSView* contentView = [window->fNSWindow contentView];
        [contentView setWantsBestResolutionOpenGLSurface:YES];
        [fGLContext setView:contentView];
    }

    void resize(int width, int height) {
        [fGLContext update];

        GLint swapInterval = 0;
        [fGLContext setValues:&swapInterval forParameter:NSOpenGLCPSwapInterval];

        [fGLContext makeCurrentContext];

        glClearStencil(0);
        glClearColor(0, 0, 0, 255);
        glStencilMask(0xffffffff);
        glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        glViewport(0, 0, width, height);
    }

    void swapBuffers() {
        [fGLContext flushBuffer];
    }

    void close() {
        [NSOpenGLContext clearCurrentContext];
        [fPixelFormat release];
        fPixelFormat = nil;
        [fGLContext release];
        fGLContext = nil;

        jwm::unref(&fWindow);
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
    jwm::LayerGL* instance = reinterpret_cast<jwm::LayerGL*>(jwm::classes::Native::fromJava(env, obj));
    jwm::WindowMac* window = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, windowObj));
    instance->attach(env, window);
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_LayerGL__1nReconfigure
  (JNIEnv* env, jobject obj, jint width, jint height) {
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_LayerGL__1nMakeCurrent
  (JNIEnv* env, jobject obj) {
    jwm::LayerGL* instance = reinterpret_cast<jwm::LayerGL*>(jwm::classes::Native::fromJava(env, obj));
    [instance->fGLContext makeCurrentContext];
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_LayerGL__1nResize
  (JNIEnv* env, jobject obj, jint width, jint height) {
    jwm::LayerGL* instance = reinterpret_cast<jwm::LayerGL*>(jwm::classes::Native::fromJava(env, obj));
    instance->resize(width, height);
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

#pragma clang diagnostic pop