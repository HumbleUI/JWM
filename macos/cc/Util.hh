#pragma once
#import <Cocoa/Cocoa.h>
#include <jni.h>

namespace jwm {
    jstring nsStringToJava(JNIEnv* env, NSString* characters);
    jobject screenFromNSScreen(JNIEnv* env, NSScreen* screen);
    NSPoint nsWindowPosition(NSWindow* window);
}