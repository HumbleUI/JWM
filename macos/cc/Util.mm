#include "Util.hh"
#include "impl/Library.hh"

jstring jwm::nsStringToJava(JNIEnv* env, NSString* characters) {
    size_t len = [characters length];
    jchar* buffer = new jchar[len];
    NSRange range = NSMakeRange(0, [characters length]);
    [characters getCharacters:buffer range:range];
    return env->NewString(buffer, len);
}

NSString* jwm::nsStringFromJava(JNIEnv* env, jstring str) {
    jsize len = env->GetStringLength(str);
    const jchar* chars = env->GetStringCritical(str, nullptr);
    NSString* res = [[NSString alloc] initWithCharacters:chars length:len];
    env->ReleaseStringCritical(str, chars);
    return res;
}

jwm::IRect jwm::transformRectRelativeToPrimaryScreen(NSRect rect, CGFloat scale) {
    NSScreen* primary = [[NSScreen screens] objectAtIndex:0];
    CGFloat primaryHeight = [primary frame].size.height;
    CGFloat primaryScale = [primary backingScaleFactor];

    return jwm::IRect::makeXYWH(
             rect.origin.x * primaryScale,
             (primaryHeight - rect.size.height - rect.origin.y) * primaryScale,
             rect.size.width * scale,
             rect.size.height * scale
           );
}

jobject jwm::screenFromNSScreen(JNIEnv* env, NSScreen* screen) {
    CGFloat scale = [screen backingScaleFactor];
    jwm::IRect bounds = jwm::transformRectRelativeToPrimaryScreen([screen frame], scale);
    jwm::IRect workArea = jwm::transformRectRelativeToPrimaryScreen([screen visibleFrame], scale);
    NSScreen* primary = [[NSScreen screens] objectAtIndex:0];
    NSNumber* id = [[screen deviceDescription] valueForKey:@"NSScreenNumber"];
    return jwm::classes::Screen::make(env, [id longValue], screen == primary, bounds, workArea, scale);
}

NSPoint jwm::nsWindowPosition(NSWindow* window) {
    NSScreen* primary = [[NSScreen screens] objectAtIndex:0];
    CGFloat primaryHeight = [primary frame].size.height;
    CGFloat primaryScale = [primary backingScaleFactor];
    
    NSScreen* screen = [window screen] ?: [NSScreen mainScreen];
    CGFloat scale = [screen backingScaleFactor];
    
    CGFloat leftGlobal = window.frame.origin.x;
    CGFloat topGlobal = primaryHeight - window.frame.size.height - window.frame.origin.y;

    CGFloat screenLeftGlobal = screen.frame.origin.x;
    CGFloat screenTopGlobal = primaryHeight - screen.frame.size.height - screen.frame.origin.y;

    jfloat left = screenLeftGlobal * primaryScale + (leftGlobal - screenLeftGlobal) * scale;
    jfloat top = screenTopGlobal * primaryScale + (topGlobal - screenTopGlobal) * scale;

    return {left, top};
}