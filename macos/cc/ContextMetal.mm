#import <QuartzCore/CAMetalLayer.h>
#import <QuartzCore/CAConstraintLayoutManager.h>
#import <Cocoa/Cocoa.h>
#import <CoreVideo/CoreVideo.h>
#include <jni.h>
#include "impl/Library.hh"
#include "MainView.hh"
#import <Metal/Metal.h>
#include "WindowMac.hh"

namespace jwm {

class ContextMetal: public Context {
public:
    ContextMetal(bool vsync, bool transact, bool useDisplayLink): fVsync(vsync) {}
    ~ContextMetal();

    void attach(Window* window) override;
    void swapBuffers() override;
    void resize() override;

    bool fVsync;
    // id<MTLDevice>       fDevice;
    // id<MTLCommandQueue> fQueue;
    MainView*           fMainView;
    // id<CAMetalDrawable> fDrawableHandle;
};

ContextMetal::~ContextMetal() {
    // // MetalWindowContext_mac
    // fMainView.layer = nil;
    // fMainView.wantsLayer = NO;
    
    // // MetalWindowContext   
    // fMetalLayer = nil;

    // CFRelease(fQueue);
    // CFRelease(fDevice);
}

// int now() {
//     return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
// }

void ContextMetal::attach(Window* window) {
    WindowMac* windowMac = reinterpret_cast<WindowMac*>(window);
    fMainView = (MainView*) [windowMac->fNSWindow contentView];

    // MetalWindowContext
    // fDevice = MTLCreateSystemDefaultDevice();
    // id<MTLDevice> device = [fMainView getDevice];
    // fQueue = [device newCommandQueue];

    // MetalWindowContext_mac
    // fMetalLayer = [CAMetalLayer layer];
    // fMetalLayer.device = fDevice;
    // fMetalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;

    this->resize();

    // fMetalLayer.allowsNextDrawableTimeout = NO;
    // fMetalLayer.displaySyncEnabled = fVsync ? YES : NO;  // TODO: need solution for 10.12 or lower
    // fMetalLayer.layoutManager = [CAConstraintLayoutManager layoutManager];
    // fMetalLayer.autoresizingMask = kCALayerHeightSizable | kCALayerWidthSizable;
    // fMetalLayer.contentsGravity = kCAGravityTopLeft;
    // if (fTransact) {
    //   fMetalLayer.needsDisplayOnBoundsChange = YES;
    //   fMetalLayer.presentsWithTransaction = YES;
    // }
    // fMetalLayer.magnificationFilter = kCAFilterNearest;
    // NSColorSpace* cs = fMainView.window.colorSpace;
    // fMetalLayer.colorspace = cs.CGColorSpace;

    // fMainView.layerContentsRedrawPolicy = NSViewLayerContentsRedrawDuringViewResize;
    // fMainView.layerContentsPlacement = NSViewLayerContentsPlacementTopLeft;
    // fMainView.layer = fMetalLayer;
    // fMainView.wantsLayer = YES;
}

void ContextMetal::resize() {
    CGFloat backingScaleFactor = jwm::backingScaleFactor(fMainView);

    CGSize backingSize = fMainView.bounds.size;
    backingSize.width *= backingScaleFactor;
    backingSize.height *= backingScaleFactor;

    // fMetalLayer.drawableSize = backingSize;
    // fMetalLayer.contentsScale = backingScaleFactor;

    fWidth = backingSize.width;
    fHeight = backingSize.height;
    fScale = jwm::backingScaleFactor(fMainView);
}

void ContextMetal::swapBuffers() {
    // id<CAMetalDrawable> currentDrawable = (id<CAMetalDrawable>) fDrawableHandle;

    // id<MTLCommandBuffer> commandBuffer([fQueue commandBuffer]);
    // // commandBuffer.label = @"Present";

    // [commandBuffer presentDrawable:currentDrawable];
    // [commandBuffer commit];

    // // ARC is off in sk_app, so we need to release the CF ref manually
    // CFRelease(fDrawableHandle);
    // fDrawableHandle = nil;
}

}


// JNI

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_ContextMetal__1nMake
  (JNIEnv* env, jclass jclass, jboolean vsync, jboolean transact, jboolean displayLink) {
    jwm::ContextMetal* instance = new jwm::ContextMetal(vsync, transact, displayLink);
    return reinterpret_cast<jlong>(instance);
}

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_ContextMetal_getDevicePtr
  (JNIEnv* env, jobject obj) {
    jwm::ContextMetal* instance = reinterpret_cast<jwm::ContextMetal*>(jwm::classes::Native::fromJava(env, obj));
    return reinterpret_cast<jlong>([instance->fMainView getDevice]);
}

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_ContextMetal_getQueuePtr
  (JNIEnv* env, jobject obj) {
    jwm::ContextMetal* instance = reinterpret_cast<jwm::ContextMetal*>(jwm::classes::Native::fromJava(env, obj));
    return reinterpret_cast<jlong>([instance->fMainView getQueue]);
}

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_ContextMetal_nextDrawableTexturePtr
  (JNIEnv* env, jobject obj) {
    jwm::ContextMetal* instance = reinterpret_cast<jwm::ContextMetal*>(jwm::classes::Native::fromJava(env, obj));
    // return reinterpret_cast<jlong>(instance->fMainView.currentDrawable.texture);
    return reinterpret_cast<jlong>(instance->fMainView);
    // id<CAMetalDrawable> currentDrawable = [instance->fMetalLayer nextDrawable];
    // instance->fDrawableHandle = (id<CAMetalDrawable>) CFRetain(currentDrawable);
    // return reinterpret_cast<jlong>(currentDrawable.texture);
}
