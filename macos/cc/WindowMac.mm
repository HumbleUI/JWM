#include <iostream>
#include <jni.h>
#include "impl/Library.hh"
#include "MainView.hh"
#include "ContextMetal.hh"
#include <memory>
#include "WindowMac.hh"
#include "WindowDelegate.hh"

jwm::WindowMac::~WindowMac() {
    if (fEventListener)
        fEnv->DeleteGlobalRef(fEventListener);
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
    NSRect windowRect = NSMakeRect(100, 100, initialWidth, initialHeight);

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

    // fWindowNumber = fNSWindow.windowNumber;
    // gWindowMap.add(this);
    return true;
}

float jwm::WindowMac::scaleFactor() const {
    NSScreen* screen = fNSWindow.contentView.window.screen ?: [NSScreen mainScreen];
    return screen.backingScaleFactor;
}

void jwm::WindowMac::onEvent(jobject event) {
    if (fEventListener)
        jwm::classes::Consumer::accept(fEnv, fEventListener, event);
}


// JNI

void jwm::deleteWindowMac(jwm::WindowMac* instance) {
    delete instance;
}

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_Window__1nGetFinalizer
  (JNIEnv* env, jclass jclass) {
    return static_cast<jlong>(reinterpret_cast<uintptr_t>(&jwm::deleteWindowMac));
}

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_Window__1nMake
  (JNIEnv* env, jclass jclass) {
    std::unique_ptr<jwm::WindowMac> instance(new jwm::WindowMac(env));
    if (instance->init())
      return reinterpret_cast<jlong>(instance.release());
    else
      return 0;
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_Window__1nSetEventListener
  (JNIEnv* env, jclass jclass, jlong ptr, jobject listener) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(static_cast<uintptr_t>(ptr));
    if (instance->fEventListener)
        env->DeleteGlobalRef(instance->fEventListener);
    instance->fEventListener = listener ? env->NewGlobalRef(listener) : nullptr;
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_Window__1nAttach
  (JNIEnv* env, jclass jclass, jlong ptr, jlong contextPtr) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(static_cast<uintptr_t>(ptr));
    jwm::Context* context = reinterpret_cast<jwm::Context*>(static_cast<uintptr_t>(contextPtr));
    context->attach(instance);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_Window__1nShow
  (JNIEnv* env, jclass jclass, jlong ptr) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(static_cast<uintptr_t>(ptr));
    [instance->fNSWindow orderFront:nil];
    [NSApp activateIgnoringOtherApps:YES];
    [instance->fNSWindow makeKeyAndOrderFront:NSApp];
}
