#include <iostream>
#include <jni.h>
#include "impl/Library.hh"
#include "MainView.hh"
#include <memory>
#include "WindowMac.hh"
#include "WindowDelegate.hh"
#include "Util.hh"

static CVReturn displayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* _now, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* ctx) {
    jwm::WindowMac* window = (jwm::WindowMac*) ctx;
    if (!window->fFrameScheduled) {
        window->fFrameScheduled = true;
        window->ref();
        dispatch_async(dispatch_get_main_queue(), ^{
            window->fFrameRequested = false;
            window->dispatch(jwm::classes::EventFrame::kInstance);
            if (!window->fFrameRequested)
                CVDisplayLinkStop(window->fDisplayLink);
            window->unref();
            window->fFrameScheduled = false;
        });
    }
    return kCVReturnSuccess;
}

jwm::WindowMac::~WindowMac() {
    [fNSWindow close];
}

bool jwm::WindowMac::init() {
    // Create a delegate to track certain events
    WindowDelegate* delegate = [[WindowDelegate alloc] initWithWindow:this];
    if (nil == delegate)
        return false;

    // Create Cocoa window
    constexpr int initialWidth = 800;
    constexpr int initialHeight = 600;
    NSRect windowRect = NSMakeRect(100 + rand() % 100, 100 + rand() % 100, initialWidth, initialHeight);

    NSUInteger windowStyle = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable | NSWindowStyleMaskMiniaturizable;

    fNSWindow = [[NSWindow alloc] initWithContentRect:windowRect styleMask:windowStyle
                                           backing:NSBackingStoreBuffered defer:NO];
    if (nil == fNSWindow) {
        [delegate release];
        return false;
    }

    // create view
    MainView* view = [[MainView alloc] initWithWindow:this];
    if (nil == view) {
        [fNSWindow release];
        [delegate release];
        return false;
    }

    [fNSWindow setContentView:view];
    [fNSWindow makeFirstResponder:view];
    [fNSWindow setDelegate:delegate];
    [fNSWindow setAcceptsMouseMovedEvents:YES];
    [fNSWindow setRestorable:NO];

    // Should be retained by window now
    [view release];

    return true;
}

void jwm::WindowMac::show() {
    [fNSWindow orderFront:nil];
    [NSApp activateIgnoringOtherApps:YES];
    [fNSWindow makeKeyAndOrderFront:NSApp];

    CVDisplayLinkCreateWithActiveCGDisplays(&fDisplayLink);
    CVDisplayLinkSetOutputCallback(fDisplayLink, &displayLinkCallback, this);
    ref(); // keep this alive during CVDisplayLink callback
}

void jwm::WindowMac::reconfigure() {
    CGDirectDisplayID currentDisplay = (CGDirectDisplayID)[[[[fNSWindow screen] deviceDescription] objectForKey:@"NSScreenNumber"] intValue];
    CGDirectDisplayID oldDisplay = CVDisplayLinkGetCurrentCGDisplay(fDisplayLink);
    if (currentDisplay != oldDisplay)
        CVDisplayLinkSetCurrentCGDisplay(fDisplayLink, currentDisplay);
}

float jwm::WindowMac::getScale() const {
    return (fNSWindow.screen ?: [NSScreen mainScreen]).backingScaleFactor;
}

void jwm::WindowMac::requestFrame() {
    fFrameRequested = true;
    if (!CVDisplayLinkIsRunning(fDisplayLink))
        CVDisplayLinkStart(fDisplayLink);
}

void jwm::WindowMac::close() {
    CVDisplayLinkStop(fDisplayLink);
    CVDisplayLinkRelease(fDisplayLink);
    unref(); // from show()
}

// JNI

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_WindowMac__1nMake
  (JNIEnv* env, jclass jclass) {
    std::unique_ptr<jwm::WindowMac> instance(new jwm::WindowMac(env));
    if (instance->init())
      return reinterpret_cast<jlong>(instance.release());
    else
      return 0;
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowMac__1nShow
  (JNIEnv* env, jobject obj) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    instance->show();
}

extern "C" JNIEXPORT jobject JNICALL Java_org_jetbrains_jwm_WindowMac__1nGetWindowRect
  (JNIEnv* env, jobject obj) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    NSWindow* nsWindow = instance->fNSWindow;
    NSPoint pos = jwm::nsWindowPosition(nsWindow);
    const NSRect frame = [nsWindow frame];
    auto scale = instance->getScale();
    return jwm::classes::UIRect::toJavaXYWH(
      env,
      pos.x,
      pos.y,
      frame.size.width * scale,
      frame.size.height * scale
    );
}

extern "C" JNIEXPORT jobject JNICALL Java_org_jetbrains_jwm_WindowMac__1nGetContentRect
  (JNIEnv* env, jobject obj) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    NSWindow* nsWindow = instance->fNSWindow;
    const NSRect frame = [nsWindow.contentView frame];
    const NSRect outerFrame = [nsWindow frame];
    auto scale = instance->getScale();
    return jwm::classes::UIRect::toJavaXYWH(
      env,
      frame.origin.x * scale,
      (outerFrame.size.height - frame.origin.y - frame.size.height) * scale,
      frame.size.width * scale,
      frame.size.height * scale
    );
}

extern "C" JNIEXPORT jboolean JNICALL Java_org_jetbrains_jwm_WindowMac__1nSetWindowPosition
  (JNIEnv* env, jobject obj, int left, int top) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    NSWindow* nsWindow = instance->fNSWindow;
    NSArray* screens = [NSScreen screens];
    for (int i = 0; i < [screens count]; ++i) {
        NSScreen* screen = [screens objectAtIndex:i];
        CGFloat scale = [screen backingScaleFactor];
        jwm::UIRect rect = jwm::transformRectRelativeToPrimaryScreen([screen frame], scale);
        if (rect.fLeft <= left && left <= rect.fRight && rect.fTop <= top && top <= rect.fBottom) {
            CGFloat relativeLeft = (left - rect.fLeft) / scale;
            CGFloat relativeTop = screen.frame.size.height - (top - rect.fTop) / scale;
            NSPoint point { screen.frame.origin.x + relativeLeft, screen.frame.origin.y + relativeTop };
            [nsWindow setFrameTopLeftPoint:point];
            return true;
        }
    }
    return false;
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowMac__1nSetWindowSize
  (JNIEnv* env, jobject obj, int width, int height) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    NSWindow* nsWindow = instance->fNSWindow;
    const NSRect oldFrame = [nsWindow frame];
    CGFloat scale = instance->getScale();
    NSRect frame = NSMakeRect(oldFrame.origin.x,
                              oldFrame.origin.y - ((CGFloat) height / scale) + oldFrame.size.height,
                              (CGFloat) width / scale,
                              (CGFloat) height / scale);
    [nsWindow setFrame:frame display:YES];
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowMac__1nSetContentSize
  (JNIEnv* env, jobject obj, int width, int height) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    NSWindow* nsWindow = instance->fNSWindow;
    CGFloat scale = instance->getScale();
    NSSize size {(CGFloat) width / scale, (CGFloat) height / scale};
    [nsWindow setContentSize:size];
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowMac__1nSetTitle
  (JNIEnv* env, jobject obj, jstring titleStr) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    jsize len = env->GetStringLength(titleStr);
    const jchar* chars = env->GetStringCritical(titleStr, nullptr);
    NSString* title = [[NSString alloc] initWithCharacters:chars length:len];
    env->ReleaseStringCritical(titleStr, chars);
    instance->fNSWindow.title = title;
    [title release];
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowMac__1nSetIcon
  (JNIEnv* env, jobject obj, jstring pathStr) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    jsize len = env->GetStringLength(pathStr);
    const jchar* chars = env->GetStringCritical(pathStr, nullptr);
    NSString* path = [[NSString alloc] initWithCharacters:chars length:len];
    env->ReleaseStringCritical(pathStr, chars);

    NSImage* image = [[NSImage alloc] initByReferencingFile:path];
    [path release];

    NSApplication* app = [NSApplication sharedApplication];
    app.applicationIconImage = image;

    [image release];
    [app release];
}

extern "C" JNIEXPORT jobject JNICALL Java_org_jetbrains_jwm_WindowMac__1nGetScreen
  (JNIEnv* env, jobject obj) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    return jwm::screenFromNSScreen(env, [instance->fNSWindow screen]);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowMac__1nRequestFrame
  (JNIEnv* env, jobject obj) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    instance->requestFrame();
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowMac__1nClose
  (JNIEnv* env, jobject obj) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    instance->close();
}
