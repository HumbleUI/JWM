#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

#include <algorithm>
#import <Cocoa/Cocoa.h>
#include "Context.hh"
#include <jni.h>
#include "impl/Library.hh"
#include "MainView.hh"
#include <OpenGL/gl.h>
#include "WindowMac.hh"

namespace jwm {

class ContextGL: public Context {
public:
    ContextGL(bool vsync, bool useDisplayLink): fVsync(vsync), fUseDisplayLink(useDisplayLink) {}
    ~ContextGL();

    void attach(Window* window) override;
    void update();
    void swapBuffers() override;
    void resize() override;

    bool fVsync;
    bool fUseDisplayLink;
    int fStencilBits;
    int fSampleCount;
    NSView* fMainView;
    NSOpenGLContext*     fGLContext;
    NSOpenGLPixelFormat* fPixelFormat;
    CVDisplayLinkRef  fDisplayLink;
    volatile int      fSwapIntervalsPassed = 0;
    id                fSwapIntervalCond;
};

ContextGL::~ContextGL() {
    if (fUseDisplayLink) {
        CVDisplayLinkStop(fDisplayLink);
        CVDisplayLinkRelease(fDisplayLink);
    }
    [NSOpenGLContext clearCurrentContext];
    [fPixelFormat release];
    fPixelFormat = nil;
    [fGLContext release];
    fGLContext = nil;
}

static CVReturn glDisplayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* _now, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* ctx) {
    ContextGL* self = (ContextGL*) ctx;
    [self->fSwapIntervalCond lock];
    self->fSwapIntervalsPassed++;
    [self->fSwapIntervalCond signal];
    [self->fSwapIntervalCond unlock];
    return kCVReturnSuccess;
}

void ContextGL::attach(Window* window) {
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

    // Setup display link.
    if (fUseDisplayLink) {
      CVDisplayLinkCreateWithActiveCGDisplays(&fDisplayLink);
      CVDisplayLinkSetOutputCallback(fDisplayLink, &glDisplayLinkCallback, this);
      CVDisplayLinkStart(fDisplayLink);
      CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(fDisplayLink,
                                                        (CGLContextObj) fGLContext,
                                                        (CGLPixelFormatObj) fPixelFormat);
      fSwapIntervalCond = [NSCondition new];
    }

    [fMainView setWantsBestResolutionOpenGLSurface:YES];
    [fGLContext setView:fMainView];

    update();
}

void ContextGL::update() {
    GLint swapInterval = fVsync ? 1 : 0;
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

void ContextGL::swapBuffers() {
    [fGLContext flushBuffer];
    if (fUseDisplayLink) {
        [this->fSwapIntervalCond lock];
        do {
            [this->fSwapIntervalCond wait];
        } while (this->fSwapIntervalsPassed == 0);
        this->fSwapIntervalsPassed = 0;
        [this->fSwapIntervalCond unlock];
    }
}

void ContextGL::resize() {
    [fGLContext update];
    update();
}

}

// JNI

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_ContextGL__1nMake
  (JNIEnv* env, jclass jclass, jboolean vsync, jboolean displayLink) {
    jwm::ContextGL* instance = new jwm::ContextGL(vsync, displayLink);
    return reinterpret_cast<jlong>(instance);
}

#pragma clang diagnostic pop