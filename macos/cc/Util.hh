#pragma once
#import <Cocoa/Cocoa.h>
#include <jni.h>
#include "impl/Library.hh"

namespace jwm {
    jstring nsStringToJava(JNIEnv* env, NSString* characters);
    NSString* nsStringFromJava(JNIEnv* env, jstring str);
    IRect transformRectRelativeToPrimaryScreen(NSRect rect, CGFloat scale);
    jobject screenFromNSScreen(JNIEnv* env, NSScreen* screen);
    NSPoint nsWindowPosition(NSWindow* window);
}