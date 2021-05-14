#import <Cocoa/Cocoa.h>
#include <jni.h>
#include "impl/Library.hh"
#include "WindowMac.hh"

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_App__1nInit
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
}

extern "C" JNIEXPORT jint JNICALL Java_org_jetbrains_jwm_App_run
  (JNIEnv* env, jclass jclass) {
    [NSApp run];
    return EXIT_SUCCESS;
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_App_terminate
  (JNIEnv* env, jclass jclass) {
    [NSApp stop:nil];
    [NSApp terminate:nil];
}