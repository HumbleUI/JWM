#pragma once
#import <Cocoa/Cocoa.h>
#include <jni.h>

namespace jwm {
    jstring nsStringToJava(JNIEnv* env, NSString* characters) {
        size_t len = [characters length];
        jchar* buffer = new jchar[len];
        NSRange range = NSMakeRange(0, [characters length]);
        [characters getCharacters:buffer range:range];
        return env->NewString(buffer, len);
    }
}