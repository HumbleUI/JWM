#import  <QuartzCore/CAMetalLayer.h>
#import  <QuartzCore/CAConstraintLayoutManager.h>
#import  <Cocoa/Cocoa.h>
#include "Context.hh"
#include <iostream>
#include <jni.h>
#include "impl/Library.hh"
#include "MainView.hh"
#import  <Metal/Metal.h>
#include "WindowMac.hh"

namespace jwm {

class ContextMetal: public Context {
public:
    ContextMetal() = default;
    ~ContextMetal() = default;

    void attach(Window* window) override;
    void invalidate() override;
    void resize() override;
    void swapBuffers() override;
    void detach() override;

    NSView* fMainView;
    id<MTLDevice>       fDevice;
    id<MTLCommandQueue> fQueue;
    CAMetalLayer*       fMetalLayer;
    id<CAMetalDrawable> fDrawableHandle;
    // id<MTLBinaryArchive> fPipelineArchive API_AVAILABLE(macos(11.0), ios(14.0));
};

void ContextMetal::attach(Window* window) {
    WindowMac* windowMac = reinterpret_cast<WindowMac*>(window);
    fMainView = [windowMac->fNSWindow contentView];

    // MetalWindowContext
    fDevice = MTLCreateSystemDefaultDevice();
    fQueue = [fDevice newCommandQueue];

    // MetalWindowContext_mac
    fMetalLayer = [CAMetalLayer layer];
    fMetalLayer.device = fDevice;
    fMetalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;

    fMetalLayer.allowsNextDrawableTimeout = NO;
    fMetalLayer.displaySyncEnabled = NO;
    fMetalLayer.layoutManager = [CAConstraintLayoutManager layoutManager];
    fMetalLayer.autoresizingMask = kCALayerHeightSizable | kCALayerWidthSizable;
    fMetalLayer.contentsGravity = kCAGravityTopLeft;
    fMetalLayer.needsDisplayOnBoundsChange = YES;
    fMetalLayer.presentsWithTransaction = YES;
    fMetalLayer.magnificationFilter = kCAFilterNearest;

    fMainView.layerContentsRedrawPolicy = NSViewLayerContentsRedrawDuringViewResize;
    fMainView.layerContentsPlacement = NSViewLayerContentsPlacementTopLeft;
    fMainView.layer = fMetalLayer;
    fMainView.wantsLayer = YES;
}

void ContextMetal::invalidate() {
    NSColorSpace* cs = fMainView.window.colorSpace;
    fMetalLayer.colorspace = cs.CGColorSpace;
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

    [commandBuffer commit];
    [commandBuffer waitUntilScheduled];
    [fDrawableHandle present];

    // ARC is off in sk_app, so we need to release the CF ref manually
    CFRelease(fDrawableHandle);
    fDrawableHandle = nil;
}

void ContextMetal::detach() {
    // MetalWindowContext_mac
    fMainView.layer = nil;
    fMainView.wantsLayer = NO;
    
    // MetalWindowContext   
    fMetalLayer = nil;

    CFRelease(fQueue);
    CFRelease(fDevice);

    fMainView = nullptr;
}

}


// JNI

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_ContextMetal__1nMake
  (JNIEnv* env, jclass jclass) {
    jwm::ContextMetal* instance = new jwm::ContextMetal();
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
