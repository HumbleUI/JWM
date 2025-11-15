#import <Cocoa/Cocoa.h>
#import <CoreGraphics/CoreGraphics.h>
#include <jni.h>

extern "C" JNIEXPORT jbyteArray JNICALL Java_io_github_humbleui_jwm_Screen__1nGetICCProfile
  (JNIEnv* env, jclass jclass, jlong screenId) {
    NSArray* screens = [NSScreen screens];
    NSScreen* targetScreen = nil;

    for (NSScreen* screen in screens) {
        NSNumber* id = [[screen deviceDescription] valueForKey:@"NSScreenNumber"];
        if ([id longValue] == screenId) {
            targetScreen = screen;
            break;
        }
    }

    if (!targetScreen) {
        return nullptr;
    }

    CGColorSpaceRef cgColorSpace = targetScreen.colorSpace.CGColorSpace;
    if (!cgColorSpace) {
        return nullptr;
    }

    CFDataRef cfIccProfile = CGColorSpaceCopyICCData(cgColorSpace);
    if (!cfIccProfile) {
        return nullptr;
    }

    const UInt8* data = CFDataGetBytePtr(cfIccProfile);
    CFIndex length = CFDataGetLength(cfIccProfile);

    jbyteArray result = env->NewByteArray(static_cast<jsize>(length));
    if (result) {
        env->SetByteArrayRegion(result, 0, static_cast<jsize>(length),
                                reinterpret_cast<const jbyte*>(data));
    }

    CFRelease(cfIccProfile);

    return result;
}
