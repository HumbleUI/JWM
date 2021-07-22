#include <iostream>
#include <jni.h>
#include "impl/Library.hh"
#include "MainView.hh"
#include <memory>
#include "WindowMac.hh"
#include "WindowDelegate.hh"

static CVReturn displayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* _now, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* ctx) {
    jwm::WindowMac* window = (jwm::WindowMac*) ctx;
    if (!window->fFrameScheduled) {
        window->fFrameScheduled = true;
        window->ref();
        dispatch_async(dispatch_get_main_queue(), ^{
            window->fFrameRequested = false;
            window->dispatch(jwm::classes::EventFrame::kInstance);
            if (!window->fFrameRequested)
                CVDisplayLinkStop(window->fDisplayLink);
            window->unref();
            window->fFrameScheduled = false;
        });
    }
    return kCVReturnSuccess;
}

jwm::WindowMac::~WindowMac() {
    [fNSWindow close];
}

bool jwm::WindowMac::init() {
    // Create a delegate to track certain events
    WindowDelegate* delegate = [[WindowDelegate alloc] initWithWindow:this];
    if (nil == delegate)
        return false;

    // Create Cocoa window
    constexpr int initialWidth = 800;
    constexpr int initialHeight = 600;
    NSRect windowRect = NSMakeRect(100 + rand() % 100, 100 + rand() % 100, initialWidth, initialHeight);

    NSUInteger windowStyle = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable | NSWindowStyleMaskMiniaturizable;

    fNSWindow = [[NSWindow alloc] initWithContentRect:windowRect styleMask:windowStyle
                                           backing:NSBackingStoreBuffered defer:NO];
    if (nil == fNSWindow) {
        [delegate release];
        return false;
    }

    // create view
    MainView* view = [[MainView alloc] initWithWindow:this];
    if (nil == view) {
        [fNSWindow release];
        [delegate release];
        return false;
    }

    [fNSWindow setContentView:view];
    [fNSWindow makeFirstResponder:view];
    [fNSWindow setDelegate:delegate];
    [fNSWindow setAcceptsMouseMovedEvents:YES];
    [fNSWindow setRestorable:NO];

    // Should be retained by window now
    [view release];

    return true;
}

void jwm::WindowMac::show() {
    [fNSWindow orderFront:nil];
    [NSApp activateIgnoringOtherApps:YES];
    [fNSWindow makeKeyAndOrderFront:NSApp];

    CVDisplayLinkCreateWithActiveCGDisplays(&fDisplayLink);
    CVDisplayLinkSetOutputCallback(fDisplayLink, &displayLinkCallback, this);
    ref(); // keep this alive during CVDisplayLink callback
}

void jwm::WindowMac::reconfigure() {
    CGDirectDisplayID currentDisplay = (CGDirectDisplayID)[[[[fNSWindow screen] deviceDescription] objectForKey:@"NSScreenNumber"] intValue];
    CGDirectDisplayID oldDisplay = CVDisplayLinkGetCurrentCGDisplay(fDisplayLink);
    if (currentDisplay != oldDisplay)
        CVDisplayLinkSetCurrentCGDisplay(fDisplayLink, currentDisplay);
}

float jwm::WindowMac::getScale() const {
    return (fNSWindow.screen ?: [NSScreen mainScreen]).backingScaleFactor;
}

void jwm::WindowMac::move(int left, int top) {
  auto screen = fNSWindow.screen ?: [NSScreen mainScreen];
  auto scale = screen.backingScaleFactor;
  NSPoint point {(CGFloat) left / scale, screen.frame.size.height - (CGFloat) top / scale};
  [fNSWindow setFrameTopLeftPoint:point];
}

void jwm::WindowMac::resize(int width, int height) {
  auto scale = getScale();
  NSSize size {(CGFloat) width / scale, (CGFloat) height / scale};
  [fNSWindow setContentSize:size];
}

void jwm::WindowMac::requestFrame() {
    fFrameRequested = true;
    if (!CVDisplayLinkIsRunning(fDisplayLink))
        CVDisplayLinkStart(fDisplayLink);
}

void jwm::WindowMac::close() {
    CVDisplayLinkStop(fDisplayLink);
    CVDisplayLinkRelease(fDisplayLink);
    unref(); // from show()
}

// JNI

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_WindowMac__1nMake
  (JNIEnv* env, jclass jclass) {
    std::unique_ptr<jwm::WindowMac> instance(new jwm::WindowMac(env));
    if (instance->init())
      return reinterpret_cast<jlong>(instance.release());
    else
      return 0;
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowMac_show
  (JNIEnv* env, jobject obj) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    instance->show();
}

extern "C" JNIEXPORT jint JNICALL Java_org_jetbrains_jwm_WindowMac_getLeft
  (JNIEnv* env, jobject obj) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    return instance->fNSWindow.frame.origin.x * instance->getScale();
}

extern "C" JNIEXPORT jint JNICALL Java_org_jetbrains_jwm_WindowMac_getTop
  (JNIEnv* env, jobject obj) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    auto window = instance->fNSWindow;
    auto screen = window.screen ?: [NSScreen mainScreen];
    return (screen.frame.size.height - window.frame.origin.y - window.frame.size.height) * instance->getScale();
}

extern "C" JNIEXPORT jint JNICALL Java_org_jetbrains_jwm_WindowMac_getWidth
  (JNIEnv* env, jobject obj) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    return instance->fNSWindow.contentView.bounds.size.width * instance->getScale();
}

extern "C" JNIEXPORT jint JNICALL Java_org_jetbrains_jwm_WindowMac_getHeight
  (JNIEnv* env, jobject obj) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    return instance->fNSWindow.contentView.bounds.size.height * instance->getScale();
}

extern "C" JNIEXPORT jfloat JNICALL Java_org_jetbrains_jwm_WindowMac_getScale
  (JNIEnv* env, jobject obj) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    return instance->getScale();
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowMac_move
  (JNIEnv* env, jobject obj, int left, int top) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    instance->move(left, top);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowMac_resize
  (JNIEnv* env, jobject obj, int width, int height) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    instance->resize(width, height);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowMac_requestFrame
  (JNIEnv* env, jobject obj) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    instance->requestFrame();
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowMac__1nRunOnUIThread
  (JNIEnv* env, jclass cls, jobject callback) {
    JavaVM* javaVM;
    env->GetJavaVM(&javaVM);
    auto callbackRef = env->NewGlobalRef(callback);
    dispatch_async(dispatch_get_main_queue(), ^{
        JNIEnv* env2;
        if (javaVM->GetEnv(reinterpret_cast<void**>(&env2), JNI_VERSION_1_8) == JNI_OK) {
          jwm::classes::Runnable::run(env2, callbackRef);
          env2->DeleteGlobalRef(callbackRef);
        }
    });
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowMac__1nClose
  (JNIEnv* env, jobject obj) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    instance->close();
}
