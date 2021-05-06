#include <jni.h>
#include "impl/Library.hh"
#include "ContextMetal.hh"
#import <QuartzCore/CAConstraintLayoutManager.h>
#include "WindowMac.hh"

jwm::ContextMetal::~ContextMetal() {
    // MetalWindowContext_mac
    fMainView.layer = nil;
    fMainView.wantsLayer = NO;
    
    // MetalWindowContext   
    fMetalLayer = nil;
    fValid = false;

    CFRelease(fQueue);
    CFRelease(fDevice);
}

void jwm::ContextMetal::attach(jwm::Window* window) {
    jwm::WindowMac* windowMac = reinterpret_cast<jwm::WindowMac*>(window);
    fMainView = [windowMac->fNSWindow contentView];

    // MetalWindowContext
    fDevice = MTLCreateSystemDefaultDevice();
    fQueue = [fDevice newCommandQueue];

    // MetalWindowContext_mac
    fMetalLayer = [CAMetalLayer layer];
    fMetalLayer.device = fDevice;
    fMetalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;

    this->resize();

    fMetalLayer.allowsNextDrawableTimeout = NO;
    fMetalLayer.displaySyncEnabled = fVsync ? YES : NO;  // TODO: need solution for 10.12 or lower
    fMetalLayer.layoutManager = [CAConstraintLayoutManager layoutManager];
    fMetalLayer.autoresizingMask = kCALayerHeightSizable | kCALayerWidthSizable;
    fMetalLayer.contentsGravity = kCAGravityTopLeft;
    if (fTransact) {
      fMetalLayer.needsDisplayOnBoundsChange = YES;
      fMetalLayer.presentsWithTransaction = YES;
    }
    fMetalLayer.magnificationFilter = kCAFilterNearest;
    NSColorSpace* cs = fMainView.window.colorSpace;
    fMetalLayer.colorspace = cs.CGColorSpace;

    fMainView.layerContentsRedrawPolicy = NSViewLayerContentsRedrawDuringViewResize;
    fMainView.layerContentsPlacement = NSViewLayerContentsPlacementTopLeft;
    fMainView.layer = fMetalLayer;
    fMainView.wantsLayer = YES;

    fValid = true;
}

void jwm::ContextMetal::resize() {
    // CGFloat backingScaleFactor = sk_app::GetBackingScaleFactor(fMainView);
    NSScreen* screen = fMainView.window.screen ?: [NSScreen mainScreen];
    CGFloat backingScaleFactor = screen.backingScaleFactor;

    CGSize backingSize = fMainView.bounds.size;
    backingSize.width *= backingScaleFactor;
    backingSize.height *= backingScaleFactor;

    fMetalLayer.drawableSize = backingSize;
    fMetalLayer.contentsScale = backingScaleFactor;

    fWidth = backingSize.width;
    fHeight = backingSize.height;
}

void jwm::ContextMetal::swapBuffers() {
    id<CAMetalDrawable> currentDrawable = (id<CAMetalDrawable>) fDrawableHandle;

    id<MTLCommandBuffer> commandBuffer([fQueue commandBuffer]);
    // commandBuffer.label = @"Present";

    if (fTransact) {
      [commandBuffer commit];
      [commandBuffer waitUntilScheduled];
      [currentDrawable present];
    } else {
      [commandBuffer presentDrawable:currentDrawable];
      [commandBuffer commit];
    }
    // ARC is off in sk_app, so we need to release the CF ref manually
    CFRelease(fDrawableHandle);
    fDrawableHandle = nil;
}


// JNI

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_macos_ContextMetal__1nMake
  (JNIEnv* env, jclass jclass, jboolean vsync, jboolean transact) {
    jwm::ContextMetal* instance = new jwm::ContextMetal(vsync, transact);
    return reinterpret_cast<jlong>(instance);
}

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_macos_ContextMetal_getDevicePtr
  (JNIEnv* env, jobject obj) {
    jwm::ContextMetal* instance = reinterpret_cast<jwm::ContextMetal*>(jwm::classes::Native::fromJava(env, obj));
    return reinterpret_cast<jlong>(instance->fDevice);
}

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_macos_ContextMetal_getQueuePtr
  (JNIEnv* env, jobject obj) {
    jwm::ContextMetal* instance = reinterpret_cast<jwm::ContextMetal*>(jwm::classes::Native::fromJava(env, obj));
    return reinterpret_cast<jlong>(instance->fQueue);
}

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_macos_ContextMetal_nextDrawableTexturePtr
  (JNIEnv* env, jobject obj) {
    jwm::ContextMetal* instance = reinterpret_cast<jwm::ContextMetal*>(jwm::classes::Native::fromJava(env, obj));
    id<CAMetalDrawable> currentDrawable = [instance->fMetalLayer nextDrawable];
    instance->fDrawableHandle = (id<CAMetalDrawable>) CFRetain(currentDrawable);
    return reinterpret_cast<jlong>(currentDrawable.texture);
}

extern "C" JNIEXPORT jint JNICALL Java_org_jetbrains_jwm_macos_ContextMetal_getWidth
  (JNIEnv* env, jobject obj) {
    jwm::ContextMetal* instance = reinterpret_cast<jwm::ContextMetal*>(jwm::classes::Native::fromJava(env, obj));
    return instance->fWidth;
}

extern "C" JNIEXPORT jint JNICALL Java_org_jetbrains_jwm_macos_ContextMetal_getHeight
  (JNIEnv* env, jobject obj) {
    jwm::ContextMetal* instance = reinterpret_cast<jwm::ContextMetal*>(jwm::classes::Native::fromJava(env, obj));
    return instance->fHeight;
}
