#import <Cocoa/Cocoa.h>
#include <jni.h>
#include "impl/JNILocal.hh"
#include "impl/Library.hh"
#include "MainView.hh"
#include "Util.hh"
#include "ApplicationDelegate.hh"

// http://trac.wxwidgets.org/ticket/13557
// here we subclass NSApplication, for the purpose of being able to override sendEvent
@interface JWMNSApplication: NSApplication {
}

- (id)init;

- (void)sendEvent:(NSEvent *)anEvent;

@end

@implementation JWMNSApplication

- (id)init {
    self = [super init];
    return self;
}

// This is needed because otherwise we don't receive any key-up events for command-key combinations (an AppKit bug, apparently)
- (void)sendEvent:(NSEvent *)anEvent {
    if ([anEvent type] == NSEventTypeKeyUp && ([anEvent modifierFlags] & NSEventModifierFlagCommand))
        [[self keyWindow] sendEvent:anEvent];
    else
        [super sendEvent:anEvent];    
}

@end

// JNI

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_App__1nInit
  (JNIEnv* env, jclass jclass) {
#if MAC_OS_X_VERSION_MAX_ALLOWED < 1070
    // we only run on systems that support at least Core Profile 3.2
    return EXIT_FAILURE;
#endif

    if (![NSThread isMainThread]) {
        jwm::classes::Throwable::throwRuntimeException(env, "App::init should be called from main thread, forgot -XstartOnFirstThread?");
        return;
    }

    [JWMNSApplication sharedApplication];

    ApplicationDelegate* delegate = [[ApplicationDelegate alloc] init];
    [NSApp setDelegate:delegate];
    [delegate release];

    jwm::initKeyTable();
    jwm::initCursorCache();
}

extern "C" JNIEXPORT jint JNICALL Java_io_github_humbleui_jwm_App__1nStart
  (JNIEnv* env, jclass jclass) {
    [NSApp run];
    return EXIT_SUCCESS;
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_App__1nTerminate
  (JNIEnv* env, jclass jclass) {
    [NSApp stop:nil];
    [NSApp terminate:nil];
}

extern "C" JNIEXPORT jobjectArray JNICALL Java_io_github_humbleui_jwm_App__1nGetScreens
  (JNIEnv* env, jclass jclass) {
    NSArray* screens = [NSScreen screens];
    jobjectArray res = env->NewObjectArray([screens count], jwm::classes::Screen::kCls, 0);
    for (int i = 0; i < [screens count]; ++i) {
      NSScreen* screen = [screens objectAtIndex:i];
      jwm::JNILocal<jobject> obj(env, jwm::screenFromNSScreen(env, screen));
      env->SetObjectArrayElement(res, i, obj.get());
    }
    return res;
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_App__1nRunOnUIThread
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
