#import <Cocoa/Cocoa.h>
#include <jni.h>
#include "impl/JNILocal.hh"
#include "impl/Library.hh"
#include "MainView.hh"
#include "Util.hh"

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_App__1nInit
  (JNIEnv* env, jclass jclass) {
#if MAC_OS_X_VERSION_MAX_ALLOWED < 1070
    // we only run on systems that support at least Core Profile 3.2
    return EXIT_FAILURE;
#endif
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
    [NSApplication sharedApplication];

    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

    //Create the application menu.
    NSMenu* menuBar=[[NSMenu alloc] initWithTitle:@"AMainMenu"];
    [NSApp setMainMenu:menuBar];

    NSMenuItem* item;
    NSMenu* subMenu;

    item=[[NSMenuItem alloc] initWithTitle:@"Apple" action:nil keyEquivalent:@""];
    [menuBar addItem:item];
    subMenu=[[NSMenu alloc] initWithTitle:@"Apple"];
    [menuBar setSubmenu:subMenu forItem:item];
    [item release];
    item=[[NSMenuItem alloc] initWithTitle:@"Quit" action:@selector(terminate:) keyEquivalent:@"q"];
    [subMenu addItem:item];
    [item release];
    [subMenu release];
    [menuBar release];
    [pool release];

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
