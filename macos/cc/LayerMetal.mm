#import  <QuartzCore/CAMetalLayer.h>
#import  <QuartzCore/CAConstraintLayoutManager.h>
#import  <Cocoa/Cocoa.h>
#include <iostream>
#include <jni.h>
#include "impl/Library.hh"
#include "Log.hh"
#import  <Metal/Metal.h>
#include "WindowMac.hh"

namespace jwm {

class LayerMetal: public RefCounted {
public:
    LayerMetal() = default;
    ~LayerMetal() = default;

    WindowMac* fWindow;
    NSView* fMainView;
    id<MTLDevice>       fDevice;
    id<MTLCommandQueue> fQueue;
    CAMetalLayer*       fMetalLayer;
    id<CAMetalDrawable> fDrawableHandle;
    // id<MTLBinaryArchive> fPipelineArchive API_AVAILABLE(macos(11.0), ios(14.0));

    void attach(JNIEnv* env, WindowMac* window) {
        fWindow = jwm::ref(window);
        fMainView = [fWindow->fNSWindow contentView];

        // MetalWindowContext
        fDevice = MTLCreateSystemDefaultDevice();
        if (nil == fDevice) {
            JWM_LOG("MTLCreateSystemDefaultDevice returned nil");
            classes::Throwable::throwLayerNotSupportedException(env, "MTLCreateSystemDefaultDevice returned nil");
            return;
        }
        fQueue = [fDevice newCommandQueue];
        if (nil == fQueue) {
            JWM_LOG("[MTLDevice newCommandQueue] returned nil");
            classes::Throwable::throwLayerNotSupportedException(env, "[MTLDevice newCommandQueue] returned nil");
            return;
        }

        // MetalWindowContext_mac
        fMetalLayer = [CAMetalLayer layer];
        if (nil == fMetalLayer) {
            JWM_LOG("[CAMetalLayer layer] returned nil");
            classes::Throwable::throwLayerNotSupportedException(env, "[CAMetalLayer layer] returned nil");
            return;
        }

        fMetalLayer.device = fDevice;
        fMetalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
        [fMetalLayer setFramebufferOnly:NO];

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

    void reconfigure() {
        NSColorSpace* cs = fMainView.window.colorSpace;
        fMetalLayer.colorspace = cs.CGColorSpace;
    }

    void resize(int width, int height) {
        fMetalLayer.contentsScale = fWindow->getScale();
        fMetalLayer.drawableSize = {(CGFloat) width, (CGFloat) height};
    }

    void swapBuffers() {
        id<MTLCommandBuffer> commandBuffer([fQueue commandBuffer]);
        commandBuffer.label = @"Present";

        [commandBuffer commit];
        [commandBuffer waitUntilScheduled];
        [fDrawableHandle present];

        // ARC is off in sk_app, so we need to release the CF ref manually
        CFRelease(fDrawableHandle);
        fDrawableHandle = nil;
    }

    void close() {
        // MetalWindowContext_mac
        fMainView.layer = nil;
        fMainView.wantsLayer = NO;
        
        // MetalWindowContext   
        fMetalLayer = nil;

        CFRelease(fQueue);
        CFRelease(fDevice);

        fMainView = nullptr;
        jwm::unref(&fWindow);
    }
};

} // namespace jwm


// JNI

extern "C" JNIEXPORT jlong JNICALL Java_io_github_humbleui_jwm_LayerMetal__1nMake
  (JNIEnv* env, jclass jclass) {
    jwm::LayerMetal* instance = new jwm::LayerMetal();
    return reinterpret_cast<jlong>(instance);
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_LayerMetal__1nAttach
  (JNIEnv* env, jobject obj, jobject windowObj) {
    jwm::LayerMetal* instance = reinterpret_cast<jwm::LayerMetal*>(jwm::classes::Native::fromJava(env, obj));
    jwm::WindowMac* window = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, windowObj));
    instance->attach(env, window);
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_LayerMetal__1nReconfigure
  (JNIEnv* env, jobject obj) {
    jwm::LayerMetal* instance = reinterpret_cast<jwm::LayerMetal*>(jwm::classes::Native::fromJava(env, obj));
    instance->reconfigure();
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_LayerMetal__1nResize
  (JNIEnv* env, jobject obj, jint width, jint height) {
    jwm::LayerMetal* instance = reinterpret_cast<jwm::LayerMetal*>(jwm::classes::Native::fromJava(env, obj));
    instance->resize(width, height);
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_LayerMetal__1nSwapBuffers
  (JNIEnv* env, jobject obj) {
    jwm::LayerMetal* instance = reinterpret_cast<jwm::LayerMetal*>(jwm::classes::Native::fromJava(env, obj));
    instance->swapBuffers();
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_LayerMetal__1nClose
  (JNIEnv* env, jobject obj) {
    jwm::LayerMetal* instance = reinterpret_cast<jwm::LayerMetal*>(jwm::classes::Native::fromJava(env, obj));
    instance->close();
}

extern "C" JNIEXPORT jlong JNICALL Java_io_github_humbleui_jwm_LayerMetal__1nGetDevicePtr
  (JNIEnv* env, jobject obj) {
    jwm::LayerMetal* instance = reinterpret_cast<jwm::LayerMetal*>(jwm::classes::Native::fromJava(env, obj));
    return reinterpret_cast<jlong>(instance->fDevice);
}

extern "C" JNIEXPORT jlong JNICALL Java_io_github_humbleui_jwm_LayerMetal__1nGetQueuePtr
  (JNIEnv* env, jobject obj) {
    jwm::LayerMetal* instance = reinterpret_cast<jwm::LayerMetal*>(jwm::classes::Native::fromJava(env, obj));
    return reinterpret_cast<jlong>(instance->fQueue);
}

extern "C" JNIEXPORT jlong JNICALL Java_io_github_humbleui_jwm_LayerMetal__1nNextDrawableTexturePtr
  (JNIEnv* env, jobject obj) {
    jwm::LayerMetal* instance = reinterpret_cast<jwm::LayerMetal*>(jwm::classes::Native::fromJava(env, obj));
    id<CAMetalDrawable> currentDrawable = [instance->fMetalLayer nextDrawable];
    instance->fDrawableHandle = (id<CAMetalDrawable>) CFRetain(currentDrawable);
    return reinterpret_cast<jlong>(currentDrawable.texture);
}
