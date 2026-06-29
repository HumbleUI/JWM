#import <Cocoa/Cocoa.h>
#include <jni.h>
#include "impl/JNILocal.hh"
#include "impl/Library.hh"
#include "Log.hh"
#include "Util.hh"
#include "JWMApplicationDelegate.hh"

#include <iostream>

// JNI

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_App__1nStart
  (JNIEnv* env, jclass jclass, jobject launcher) {
#if MAC_OS_X_VERSION_MAX_ALLOWED < 1070
    // we only run on systems that support at least Core Profile 3.2
    return EXIT_FAILURE;
#endif

    JavaVM* jvm;
    env->GetJavaVM(&jvm);
    jobject launcherRef = env->NewGlobalRef(launcher);
    JWMApplicationDelegate* delegate = [[JWMApplicationDelegate alloc] initWithJVM:jvm andLauncherGlobalRef:launcherRef];

    // Start the application on the main thread, blocking here until complete.
    // If we are already on the main thread then this will simply call the selector.
    [delegate performSelectorOnMainThread:@selector(runLoop) withObject:nil waitUntilDone:YES];
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_App__1nTerminate
  (JNIEnv* env, jclass jclass) {
    // https://github.com/glfw/glfw/blob/ed6452b13c76f7b4da216a9952bc7837aeb0f031/src/cocoa_init.m#L439-L443
    // https://github.com/glfw/glfw/blob/ed6452b13c76f7b4da216a9952bc7837aeb0f031/src/cocoa_window.m#L1599-L1615
    //
    // Terminate by making [NSApp run] return, not via [NSApp terminate:].
    // [NSApp terminate:] calls exit(), which hard-kills the process and skips
    // the JVM's normal shutdown — no shutdown hooks, no clean DestroyJavaVM
    //
    // -stop: only takes effect after the loop processes one more event, so we
    // post a no-op event to wake it immediately.

    [NSApp stop:nil];
    NSEvent* event = [NSEvent otherEventWithType:NSEventTypeApplicationDefined
                                        location:NSMakePoint(0, 0)
                                   modifierFlags:0
                                       timestamp:0
                                    windowNumber:0
                                         context:nil
                                         subtype:0
                                           data1:0
                                           data2:0];
    [NSApp postEvent:event atStart:YES];
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
        jint ret = javaVM->GetEnv(reinterpret_cast<void**>(&env2), JNI_VERSION_1_8);
        if (ret == JNI_OK) {
            jwm::classes::Runnable::run(env2, callbackRef);
            env2->DeleteGlobalRef(callbackRef);
        } else
            std::cerr << "Failed to AttachCurrentThread: " << ret << std::endl;
    });
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_App__1nOpenSymbolsPalette
  (JNIEnv* env, jclass cls) {
    NSApplication* app = [NSApplication sharedApplication];
    [app orderFrontCharacterPalette:nil];
}
