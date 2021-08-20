#pragma once
#import <Cocoa/Cocoa.h>
#include <jni.h>
#include "impl/Library.hh"

namespace jwm {
    jstring nsStringToJava(JNIEnv* env, NSString* characters);
    UIRect nsScreenRect(NSScreen* screen);
    jobject screenFromNSScreen(JNIEnv* env, NSScreen* screen);
    NSPoint nsWindowPosition(NSWindow* window);
}