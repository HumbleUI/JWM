#include <iostream>
#include <jni.h>
#include <sstream>
#include "impl/Library.hh"
#include "MainView.hh"
#include "MouseCursor.hh"
#include <memory>
#include "WindowMac.hh"
#include "WindowDelegate.hh"
#include "Util.hh"
#include "ZOrder.hh"
#include "WindowMacTitlebarStyle.hh"

namespace jwm {
NSArray* kCursorCache;

NSCursor* cursorFromFile(NSString* name) {
    NSString *base = @"/System/Library/Frameworks/ApplicationServices.framework/Versions/A/Frameworks/HIServices.framework/Versions/A/Resources/cursors";
    NSString *path = [base stringByAppendingPathComponent:name];
    NSString *file = [path stringByAppendingPathComponent:@"cursor.pdf"];
    NSString *info = [path stringByAppendingPathComponent:@"info.plist"];
    NSImage  *image = [[NSImage alloc] initByReferencingFile:file];
    NSDictionary *dict = [NSDictionary dictionaryWithContentsOfFile:info];
    NSPoint   hotspot = NSMakePoint([[dict valueForKey:@"hotx"] doubleValue],
                                    [[dict valueForKey:@"hoty"] doubleValue]);
    [dict release];
    [info release];
    [file release];
    [path release];
    return [[NSCursor alloc] initWithImage:image hotSpot:hotspot];
}

void initCursorCache() {
    // must be in sync with MouseCursor.hh
    kCursorCache = [NSArray arrayWithObjects:
                    [NSCursor arrowCursor],        /* ARROW */
                    [NSCursor crosshairCursor],    /* CROSSHAIR */
                    cursorFromFile(@"help"),       /* HELP */
                    [NSCursor pointingHandCursor], /* POINTING_HAND */
                    [NSCursor IBeamCursor],        /* IBEAM */
                    cursorFromFile(@"notallowed"), /* NOT_ALLOWED */
                    [NSCursor arrowCursor],        /* WAIT */
                    [NSCursor arrowCursor],        /* WIN_UPARROW */
                    nil];
    [kCursorCache retain];
}
}

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

void jwm::WindowMac::setVisible(bool value) {
    if (value && fDisplayLink == 0) {
        [fNSWindow orderFront:nil];
        [NSApp activateIgnoringOtherApps:YES];
        [fNSWindow makeKeyAndOrderFront:NSApp];

        CVDisplayLinkCreateWithActiveCGDisplays(&fDisplayLink);
        CVDisplayLinkSetOutputCallback(fDisplayLink, &displayLinkCallback, this);
        ref(); // keep this alive during CVDisplayLink callback
    } else if (!value && fDisplayLink != 0) {
        [fNSWindow orderOut:fNSWindow];
        CVDisplayLinkStop(fDisplayLink);
        CVDisplayLinkRelease(fDisplayLink);
        fDisplayLink = 0;
        unref();
    }
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

bool jwm::WindowMac::isMaximized() {
    return NSEqualRects(fNSWindow.screen.visibleFrame, fNSWindow.frame);
}

void jwm::WindowMac::close() {
    if (fDisplayLink != 0) {
        CVDisplayLinkStop(fDisplayLink);
        CVDisplayLinkRelease(fDisplayLink);
        unref(); // from setVisible()
    }
}

// JNI

extern "C" JNIEXPORT jlong JNICALL Java_io_github_humbleui_jwm_WindowMac__1nMake
  (JNIEnv* env, jclass jclass) {
    std::unique_ptr<jwm::WindowMac> instance(new jwm::WindowMac(env));
    if (instance->init())
      return reinterpret_cast<jlong>(instance.release());
    else
      return 0;
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowMac__1nSetVisible
  (JNIEnv* env, jobject obj, jboolean value) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    instance->setVisible(value);
}

extern "C" JNIEXPORT jobject JNICALL Java_io_github_humbleui_jwm_WindowMac__1nGetWindowRect
  (JNIEnv* env, jobject obj) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    NSWindow* nsWindow = instance->fNSWindow;
    NSPoint pos = jwm::nsWindowPosition(nsWindow);
    const NSRect frame = [nsWindow frame];
    auto scale = instance->getScale();
    return jwm::classes::IRect::toJavaXYWH(
      env,
      pos.x,
      pos.y,
      frame.size.width * scale,
      frame.size.height * scale
    );
}

extern "C" JNIEXPORT jobject JNICALL Java_io_github_humbleui_jwm_WindowMac__1nGetContentRect
  (JNIEnv* env, jobject obj) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    NSWindow* nsWindow = instance->fNSWindow;
    const NSRect frame = [nsWindow.contentView frame];
    const NSRect outerFrame = [nsWindow frame];
    auto scale = instance->getScale();
    return jwm::classes::IRect::toJavaXYWH(
      env,
      frame.origin.x * scale,
      (outerFrame.size.height - frame.origin.y - frame.size.height) * scale,
      frame.size.width * scale,
      frame.size.height * scale
    );
}

extern "C" JNIEXPORT jboolean JNICALL Java_io_github_humbleui_jwm_WindowMac__1nSetWindowPosition
  (JNIEnv* env, jobject obj, int left, int top) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    NSWindow* nsWindow = instance->fNSWindow;
    NSArray* screens = [NSScreen screens];
    for (int i = 0; i < [screens count]; ++i) {
        NSScreen* screen = [screens objectAtIndex:i];
        CGFloat scale = [screen backingScaleFactor];
        jwm::IRect rect = jwm::transformRectRelativeToPrimaryScreen([screen frame], scale);
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

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowMac__1nSetWindowSize
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

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowMac__1nSetContentSize
  (JNIEnv* env, jobject obj, int width, int height) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    NSWindow* nsWindow = instance->fNSWindow;
    CGFloat scale = instance->getScale();
    NSSize size {(CGFloat) width / scale, (CGFloat) height / scale};
    [nsWindow setContentSize:size];
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowMac__1nSetTitle
  (JNIEnv* env, jobject obj, jstring titleStr) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    if (env->IsSameObject(titleStr, nullptr)) {
        [instance->fNSWindow setTitleVisibility:NSWindowTitleHidden];
    } else {
        jsize len = env->GetStringLength(titleStr);
        const jchar* chars = env->GetStringCritical(titleStr, nullptr);
        NSString* title = [[NSString alloc] initWithCharacters:chars length:len];
        env->ReleaseStringCritical(titleStr, chars);
        instance->fNSWindow.title = title;
        [instance->fNSWindow setTitleVisibility:NSWindowTitleVisible];
        [title release];
    }
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowMac__1nSetSubtitle
  (JNIEnv* env, jobject obj, jstring subtitleStr) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    jsize len = env->GetStringLength(subtitleStr);
    const jchar* chars = env->GetStringCritical(subtitleStr, nullptr);
    NSString* subtitle = [[NSString alloc] initWithCharacters:chars length:len];
    env->ReleaseStringCritical(subtitleStr, chars);
    instance->fNSWindow.subtitle = subtitle;
    [instance->fNSWindow setTitleVisibility:NSWindowTitleVisible];
    [subtitle release];
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowMac__1nSetIcon
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
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowMac__1nSetTitlebarVisible
        (JNIEnv* env, jobject obj, jboolean value) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    NSWindow* nsWindow = instance->fNSWindow;

    NSWindowStyleMask style = [nsWindow styleMask];
    if (value) {
        style |= NSWindowStyleMaskTitled;
    } else {
        style &= ~NSWindowStyleMaskTitled;
    }
    [nsWindow setStyleMask:style];
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowMac__1nSetFullSizeContentView
        (JNIEnv* env, jobject obj, jboolean value) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    NSWindow* nsWindow = instance->fNSWindow;

    NSWindowStyleMask style = [nsWindow styleMask];
    if (value) {
        style |= NSWindowStyleMaskFullSizeContentView;
    } else {
        style &= ~NSWindowStyleMaskFullSizeContentView;
    }
    [nsWindow setStyleMask:style];
    [nsWindow setTitlebarAppearsTransparent:value];
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowMac__1nSetTitlebarStyle
        (JNIEnv* env, jobject obj, jint titlebarStyle) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    NSWindow* nsWindow = instance->fNSWindow;

    NSToolbar* toolbar = nullptr;
    NSWindowToolbarStyle toolbarStyle = NSWindowToolbarStyleAutomatic;
    switch (static_cast<jwm::WindowMacTitlebarStyle>(titlebarStyle)) {
        case jwm::WindowMacTitlebarStyle::DEFAULT:
            // Just set the toolbar to null
            break;
        case jwm::WindowMacTitlebarStyle::UNIFIED_SMALL:
            toolbar = [[NSToolbar alloc] init];
            toolbarStyle = NSWindowToolbarStyleUnifiedCompact;
            break;
        case jwm::WindowMacTitlebarStyle::UNIFIED_LARGE:
            toolbar = [[NSToolbar alloc] init];
            toolbarStyle = NSWindowToolbarStyleUnified;
            break;
    }
    [nsWindow setToolbar:toolbar];
    [nsWindow setToolbarStyle:toolbarStyle];
    [toolbar release];
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowMac__1nSetTrafficLightPosition
        (JNIEnv* env, jobject obj, jint left, jint top) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    NSWindow* nsWindow = instance->fNSWindow;

//todo
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowMac__1nSetMouseCursor
  (JNIEnv* env, jobject obj, jint cursorIdx) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    NSCursor* cursor = [jwm::kCursorCache objectAtIndex:cursorIdx];
    [cursor set];
}

extern "C" JNIEXPORT jobject JNICALL Java_io_github_humbleui_jwm_WindowMac__1nGetScreen
  (JNIEnv* env, jobject obj) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    return jwm::screenFromNSScreen(env, [instance->fNSWindow screen]);
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowMac__1nRequestFrame
  (JNIEnv* env, jobject obj) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    instance->requestFrame();
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowMac__1nMaximize
  (JNIEnv* env, jobject obj) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    NSWindow* nsWindow = instance->fNSWindow;
    if (!instance->isMaximized()) {
        instance->fRestoreFrame = nsWindow.frame;
        [nsWindow setFrame:nsWindow.screen.visibleFrame display:YES];
        instance->dispatch(jwm::classes::EventWindowMaximize::kInstance);
    }
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowMac__1nMinimize
  (JNIEnv* env, jobject obj) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    if (!instance->fNSWindow.miniaturized) {
        [instance->fNSWindow miniaturize:nil];
        instance->dispatch(jwm::classes::EventWindowMinimize::kInstance);
    }
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowMac__1nRestore
  (JNIEnv* env, jobject obj) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    NSWindow* nsWindow = instance->fNSWindow;
    if (nsWindow.miniaturized) {
        [nsWindow deminiaturize:nil];
        instance->dispatch(jwm::classes::EventWindowRestore::kInstance);
    } else if (instance->isMaximized() && !NSEqualRects(NSZeroRect, instance->fRestoreFrame)) {
        [nsWindow setFrame:instance->fRestoreFrame display:YES];
        instance->fRestoreFrame = NSZeroRect;
        instance->dispatch(jwm::classes::EventWindowRestore::kInstance);
    }
}

extern "C" JNIEXPORT jint JNICALL Java_io_github_humbleui_jwm_WindowMac__1nGetZOrder
  (JNIEnv* env, jobject obj) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    NSWindow* nsWindow = instance->fNSWindow;
    NSWindowLevel level = [nsWindow level];

    if (level <= NSNormalWindowLevel)
        return static_cast<jint>(jwm::ZOrder::NORMAL);
    else if (level <= NSFloatingWindowLevel)
        return static_cast<jint>(jwm::ZOrder::FLOATING);
    else if (level <= NSModalPanelWindowLevel)
        return static_cast<jint>(jwm::ZOrder::MODAL_PANEL);
    else if (level <= NSMainMenuWindowLevel)
        return static_cast<jint>(jwm::ZOrder::MAIN_MENU);
    else if (level <= NSStatusWindowLevel)
        return static_cast<jint>(jwm::ZOrder::STATUS);
    else if (level <= NSPopUpMenuWindowLevel)
        return static_cast<jint>(jwm::ZOrder::POP_UP_MENU);
    else if (level <= NSScreenSaverWindowLevel)
        return static_cast<jint>(jwm::ZOrder::SCREEN_SAVER);
    else {
        std::ostringstream oss;
        oss << "Unexpected NSWindowLevel: " << level;
        jwm::classes::Throwable::throwRuntimeException(env, oss.str().c_str());
        return -1;
    }
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowMac__1nSetZOrder
  (JNIEnv* env, jobject obj, jint order) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    NSWindow* nsWindow = instance->fNSWindow;
    NSWindowLevel level;
    switch (static_cast<jwm::ZOrder>(order)) {
        case jwm::ZOrder::NORMAL:
            level = NSNormalWindowLevel;
            break;
        case jwm::ZOrder::FLOATING:
            level = NSFloatingWindowLevel;
            break;
        case jwm::ZOrder::MODAL_PANEL:
            level = NSModalPanelWindowLevel;
            break;
        case jwm::ZOrder::MAIN_MENU:
            level = NSMainMenuWindowLevel;
            break;
        case jwm::ZOrder::STATUS:
            level = NSStatusWindowLevel;
            break;
        case jwm::ZOrder::POP_UP_MENU:
            level = NSPopUpMenuWindowLevel;
            break;
        case jwm::ZOrder::SCREEN_SAVER:
            level = NSScreenSaverWindowLevel;
            break;
    }
    nsWindow.level = level;
}
extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowMac__1nClose
  (JNIEnv* env, jobject obj) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    instance->close();
}
