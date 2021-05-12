#import  <QuartzCore/CAMetalLayer.h>
#import  <QuartzCore/CAConstraintLayoutManager.h>
#import  <Cocoa/Cocoa.h>
#include "ContextMac.hh"
#import  <CoreVideo/CoreVideo.h>
#include <iostream>
#include <jni.h>
#include "impl/Library.hh"
#include "MainView.hh"
#import  <Metal/Metal.h>
#include "WindowMac.hh"

namespace jwm {

class ContextMetal: public ContextMac {
public:
    ContextMetal(bool useVsync, bool useDisplayLink, bool useTransact): ContextMac(useVsync, useDisplayLink), fUseTransact(useTransact) {}
    ~ContextMetal();

    void attach(Window* window) override;
    void reinit() override;
    void resize() override;
    void swapBuffers() override;

    bool fUseTransact;
    id<MTLDevice>       fDevice;
    id<MTLCommandQueue> fQueue;
    CAMetalLayer*       fMetalLayer;
    id<CAMetalDrawable> fDrawableHandle;
    // id<MTLBinaryArchive> fPipelineArchive API_AVAILABLE(macos(11.0), ios(14.0));
};

ContextMetal::~ContextMetal() {
    // MetalWindowContext_mac
    fMainView.layer = nil;
    fMainView.wantsLayer = NO;
    
    // MetalWindowContext   
    fMetalLayer = nil;

    CFRelease(fQueue);
    CFRelease(fDevice);
}

void ContextMetal::attach(Window* window) {
    ContextMac::attach(window);

    // MetalWindowContext
    fDevice = MTLCreateSystemDefaultDevice();
    fQueue = [fDevice newCommandQueue];

    // MetalWindowContext_mac
    fMetalLayer = [CAMetalLayer layer];
    fMetalLayer.device = fDevice;
    fMetalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;

    fMetalLayer.allowsNextDrawableTimeout = NO;
    fMetalLayer.displaySyncEnabled = fUseVsync ? YES : NO;  // TODO: need solution for 10.12 or lower
    fMetalLayer.layoutManager = [CAConstraintLayoutManager layoutManager];
    fMetalLayer.autoresizingMask = kCALayerHeightSizable | kCALayerWidthSizable;
    fMetalLayer.contentsGravity = kCAGravityTopLeft;
    if (fUseTransact) {
      fMetalLayer.needsDisplayOnBoundsChange = YES;
      fMetalLayer.presentsWithTransaction = YES;
    }
    fMetalLayer.magnificationFilter = kCAFilterNearest;

    fMainView.layerContentsRedrawPolicy = NSViewLayerContentsRedrawDuringViewResize;
    fMainView.layerContentsPlacement = NSViewLayerContentsPlacementTopLeft;
    fMainView.layer = fMetalLayer;
    fMainView.wantsLayer = YES;

    reinit();
}

void ContextMetal::reinit() {
    ContextMac::reinit();

    NSColorSpace* cs = fMainView.window.colorSpace;
    fMetalLayer.colorspace = cs.CGColorSpace;

    resize();
}

void ContextMetal::resize() {
    CGFloat backingScaleFactor = jwm::backingScaleFactor(fMainView);

    CGSize backingSize = fMainView.bounds.size;
    backingSize.width *= backingScaleFactor;
    backingSize.height *= backingScaleFactor;

    fMetalLayer.drawableSize = backingSize;
    fMetalLayer.contentsScale = backingScaleFactor;

    fWidth = backingSize.width;
    fHeight = backingSize.height;
    fScale = jwm::backingScaleFactor(fMainView);
}

void ContextMetal::swapBuffers() {
    id<MTLCommandBuffer> commandBuffer([fQueue commandBuffer]);
    commandBuffer.label = @"Present";

    if (fUseTransact) {
      [commandBuffer commit];
      [commandBuffer waitUntilScheduled];
      [fDrawableHandle present];
    } else {
      [commandBuffer presentDrawable:fDrawableHandle];
      [commandBuffer commit];
    }
    // ARC is off in sk_app, so we need to release the CF ref manually
    CFRelease(fDrawableHandle);
    fDrawableHandle = nil;
    
    ContextMac::swapBuffers();
}

}


// JNI

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_ContextMetal__1nMake
  (JNIEnv* env, jclass jclass, jboolean vsync, jboolean displayLink, jboolean transact) {
    jwm::ContextMetal* instance = new jwm::ContextMetal(vsync, displayLink, transact);
    return reinterpret_cast<jlong>(instance);
}

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_ContextMetal_getDevicePtr
  (JNIEnv* env, jobject obj) {
    jwm::ContextMetal* instance = reinterpret_cast<jwm::ContextMetal*>(jwm::classes::Native::fromJava(env, obj));
    return reinterpret_cast<jlong>(instance->fDevice);
}

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_ContextMetal_getQueuePtr
  (JNIEnv* env, jobject obj) {
    jwm::ContextMetal* instance = reinterpret_cast<jwm::ContextMetal*>(jwm::classes::Native::fromJava(env, obj));
    return reinterpret_cast<jlong>(instance->fQueue);
}

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_ContextMetal_nextDrawableTexturePtr
  (JNIEnv* env, jobject obj) {
    jwm::ContextMetal* instance = reinterpret_cast<jwm::ContextMetal*>(jwm::classes::Native::fromJava(env, obj));
    id<CAMetalDrawable> currentDrawable = [instance->fMetalLayer nextDrawable];
    instance->fDrawableHandle = (id<CAMetalDrawable>) CFRetain(currentDrawable);
    return reinterpret_cast<jlong>(currentDrawable.texture);
}
