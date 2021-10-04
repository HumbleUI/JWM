#import <Cocoa/Cocoa.h>
#include <jni.h>

const CFStringRef WhiteOnBlack = CFSTR("whiteOnBlack");
const CFStringRef UniversalAccessDomain = CFSTR("com.apple.universalaccess");

extern "C" JNIEXPORT bool JNICALL Java_io_github_humbleui_jwm_Theme__1nIsHighContrast
  (JNIEnv* env, jclass jclass) {
    NSWorkspace* workspace = [NSWorkspace sharedWorkspace];
    if ([workspace respondsToSelector:@selector (accessibilityDisplayShouldIncreaseContrast)]) {
        return workspace.accessibilityDisplayShouldIncreaseContrast;
    }
    return false;
}

extern "C" JNIEXPORT bool JNICALL Java_io_github_humbleui_jwm_Theme__1nIsDark
  (JNIEnv* env, jclass jclass) {
    if (@available(macOS 10.14, *)) {
        NSAppearanceName appearance = [[NSApp effectiveAppearance] bestMatchFromAppearancesWithNames:@[
            NSAppearanceNameAqua, NSAppearanceNameDarkAqua
        ]];
        return [appearance isEqual:NSAppearanceNameDarkAqua];
    }
    return false;
}

extern "C" JNIEXPORT bool JNICALL Java_io_github_humbleui_jwm_Theme__1nIsInverted
  (JNIEnv* env, jclass jclass) {
    CFPreferencesAppSynchronize(UniversalAccessDomain);
    Boolean keyExistsAndHasValidFormat = false;
    Boolean is_inverted = CFPreferencesGetAppBooleanValue(WhiteOnBlack, UniversalAccessDomain, &keyExistsAndHasValidFormat);
    if (!keyExistsAndHasValidFormat)
        return false;
    return is_inverted;
}
