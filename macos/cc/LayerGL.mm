#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

#include <algorithm>
#import  <Cocoa/Cocoa.h>
#include "Layer.hh"
#include <iostream>
#include <jni.h>
#include "impl/Library.hh"
#include "MainView.hh"
#include <OpenGL/gl.h>
#include "WindowMac.hh"

namespace jwm {

class LayerGL: public Layer {
public:
    LayerGL() = default;
    ~LayerGL() = default;

    void attach(Window* window) override;
    void invalidate() override;
    void resize() override;
    void swapBuffers() override;
    void detach() override;

    NSView* fMainView;
    int fStencilBits;
    int fSampleCount;
    NSOpenGLContext* fGLContext;
    NSOpenGLPixelFormat* fPixelFormat;
};

void LayerGL::attach(Window* window) {
    WindowMac* windowMac = reinterpret_cast<WindowMac*>(window);
    fMainView = [windowMac->fNSWindow contentView];

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
        return;
    }

    // create context
    fGLContext = [[NSOpenGLContext alloc] initWithFormat:fPixelFormat shareContext:nil];
    if (nil == fGLContext) {
        [fPixelFormat release];
        fPixelFormat = nil;
        return;
    }

    [fMainView setWantsBestResolutionOpenGLSurface:YES];
    [fGLContext setView:fMainView];
}

void LayerGL::invalidate() {
}

void LayerGL::resize() {
    [fGLContext update];

    GLint swapInterval = 0;
    [fGLContext setValues:&swapInterval forParameter:NSOpenGLCPSwapInterval];

    [fGLContext makeCurrentContext];

    glClearStencil(0);
    glClearColor(0, 0, 0, 255);
    glStencilMask(0xffffffff);
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    GLint stencilBits;
    [fPixelFormat getValues:&stencilBits forAttribute:NSOpenGLPFAStencilSize forVirtualScreen:0];
    fStencilBits = stencilBits;
    GLint sampleCount;
    [fPixelFormat getValues:&sampleCount forAttribute:NSOpenGLPFASamples forVirtualScreen:0];
    fSampleCount = sampleCount;
    fSampleCount = std::max(fSampleCount, 1);

    CGFloat backingScaleFactor = jwm::backingScaleFactor(fMainView);
    fWidth = fMainView.bounds.size.width * backingScaleFactor;
    fHeight = fMainView.bounds.size.height * backingScaleFactor;
    fScale = jwm::backingScaleFactor(fMainView);
    glViewport(0, 0, fWidth, fHeight);
}

void LayerGL::swapBuffers() {
    [fGLContext flushBuffer];
}

void LayerGL::detach() {
    [NSOpenGLContext clearCurrentContext];
    [fPixelFormat release];
    fPixelFormat = nil;
    [fGLContext release];
    fGLContext = nil;

    fMainView = nullptr;
}

}

// JNI

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_LayerGL__1nMake
  (JNIEnv* env, jclass jclass) {
    jwm::LayerGL* instance = new jwm::LayerGL();
    return reinterpret_cast<jlong>(instance);
}

#pragma clang diagnostic pop