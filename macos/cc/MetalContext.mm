#include <jni.h>
#include "impl/Library.hh"
#include "MetalContext.hh"
#import <QuartzCore/CAConstraintLayoutManager.h>

jwm::MetalContext::~MetalContext() {
    // MetalWindowContext_mac
    fMainView.layer = nil;
    fMainView.wantsLayer = NO;
    
    // MetalWindowContext   
    fMetalLayer = nil;
    fValid = false;

    CFRelease(fQueue);
    CFRelease(fDevice);
}

void jwm::MetalContext::attach(NSView* mainView) {
    fMainView = mainView;

    // MetalWindowContext
    fDevice = MTLCreateSystemDefaultDevice();
    fQueue = [fDevice newCommandQueue];

    // MetalWindowContext_mac
    fMetalLayer = [CAMetalLayer layer];
    fMetalLayer.device = fDevice;
    fMetalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;

    this->resize();

    fMetalLayer.displaySyncEnabled = fVsync ? YES : NO;  // TODO: need solution for 10.12 or lower
    fMetalLayer.layoutManager = [CAConstraintLayoutManager layoutManager];
    fMetalLayer.autoresizingMask = kCALayerHeightSizable | kCALayerWidthSizable;
    fMetalLayer.contentsGravity = kCAGravityTopLeft;
    // fMetalLayer.needsDisplayOnBoundsChange = YES;
    // fMetalLayer.presentsWithTransaction = YES;
    fMetalLayer.magnificationFilter = kCAFilterNearest;
    NSColorSpace* cs = fMainView.window.colorSpace;
    fMetalLayer.colorspace = cs.CGColorSpace;

    fMainView.layer = fMetalLayer;
    fMainView.wantsLayer = YES;

    fValid = true;
}

void jwm::MetalContext::resize() {
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

void jwm::MetalContext::swapBuffers() {
    id<CAMetalDrawable> currentDrawable = (id<CAMetalDrawable>) fDrawableHandle;

    id<MTLCommandBuffer> commandBuffer([fQueue commandBuffer]);
    commandBuffer.label = @"Present";

    [commandBuffer presentDrawable:currentDrawable];
    [commandBuffer commit];
    // ARC is off in sk_app, so we need to release the CF ref manually
    CFRelease(fDrawableHandle);
    fDrawableHandle = nil;
}

// JNI

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_macos_MetalContext__1nMake
  (JNIEnv* env, jclass jclass, jboolean vsync) {
    jwm::MetalContext* instance = new jwm::MetalContext(vsync);
    return reinterpret_cast<jlong>(instance);
}

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_macos_MetalContext_getDevicePtr
  (JNIEnv* env, jobject obj) {
    jwm::MetalContext* instance = reinterpret_cast<jwm::MetalContext*>(jwm::classes::Native::fromJava(env, obj));
    return reinterpret_cast<jlong>(instance->fDevice);
}

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_macos_MetalContext_getQueuePtr
  (JNIEnv* env, jobject obj) {
    jwm::MetalContext* instance = reinterpret_cast<jwm::MetalContext*>(jwm::classes::Native::fromJava(env, obj));
    return reinterpret_cast<jlong>(instance->fQueue);
}

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_macos_MetalContext_nextDrawableTexturePtr
  (JNIEnv* env, jobject obj) {
    jwm::MetalContext* instance = reinterpret_cast<jwm::MetalContext*>(jwm::classes::Native::fromJava(env, obj));
    id<CAMetalDrawable> currentDrawable = [instance->fMetalLayer nextDrawable];
    instance->fDrawableHandle = (id<CAMetalDrawable>) CFRetain(currentDrawable);
    return reinterpret_cast<jlong>(currentDrawable.texture);
}

extern "C" JNIEXPORT jint JNICALL Java_org_jetbrains_jwm_macos_MetalContext_getWidth
  (JNIEnv* env, jobject obj) {
    jwm::MetalContext* instance = reinterpret_cast<jwm::MetalContext*>(jwm::classes::Native::fromJava(env, obj));
    return instance->fWidth;
}

extern "C" JNIEXPORT jint JNICALL Java_org_jetbrains_jwm_macos_MetalContext_getHeight
  (JNIEnv* env, jobject obj) {
    jwm::MetalContext* instance = reinterpret_cast<jwm::MetalContext*>(jwm::classes::Native::fromJava(env, obj));
    return instance->fHeight;
}
