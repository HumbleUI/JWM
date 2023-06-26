#include <iostream>
#include <jni.h>
#include <sstream>
#include "impl/Library.hh"
#include "JWMMainView.hh"
#include "MouseCursor.hh"
#include <memory>
#include "WindowMac.hh"
#include "JWMWindowDelegate.hh"
#include "Util.hh"
#include "ZOrder.hh"
#include "WindowMacTitlebarStyle.hh"
#include "DockTileProgressBar.hh"

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
                    [NSCursor arrowCursor],           /* ARROW */
                    [NSCursor crosshairCursor],       /* CROSSHAIR */
                    cursorFromFile(@"help"),          /* HELP */
                    [NSCursor pointingHandCursor],    /* POINTING_HAND */
                    [NSCursor IBeamCursor],           /* IBEAM */
                    cursorFromFile(@"notallowed"),    /* NOT_ALLOWED */
                    [NSCursor arrowCursor],           /* WAIT */
                    [NSCursor arrowCursor],           /* WIN_UPARROW */
                    [NSCursor resizeUpDownCursor],    /* RESIZE_NS */
                    [NSCursor resizeLeftRightCursor], /* RESIZE_WE */
                    [NSCursor pointingHandCursor],    /* RESIZE_NESW */ // FIXME
                    [NSCursor pointingHandCursor],    /* RESIZE_NWSE */ // FIXME
                    nil];
    [kCursorCache retain];
}
}

static CVReturn displayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* _now, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* ctx) {
    jwm::WindowMac* window = (jwm::WindowMac*) ctx;
    if (window->fFrameRequested && !window->fFrameScheduled) {
        window->fFrameScheduled = true;
        window->ref();
        dispatch_async(dispatch_get_main_queue(), ^{
            window->fFrameRequested = false;
            window->dispatch(jwm::classes::EventFrame::kInstance);
            if (!window->fFrameRequested) {
              std::lock_guard<std::mutex> lock(window->fDisplayLinkMutex);
              if (!window->fFrameRequested && window->fDisplayLinkRunning) {
                CVDisplayLinkStop(window->fDisplayLink);
                window->fDisplayLinkRunning = false;
              }
            }
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
    JWMWindowDelegate* delegate = [[JWMWindowDelegate alloc] initWithWindow:this];
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

    fCursor = [kCursorCache objectAtIndex:0];

    // create view
    JWMMainView* view = [[JWMMainView alloc] initWithWindow:this];
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
    std::lock_guard<std::mutex> lock(fDisplayLinkMutex);
    if (value && fDisplayLink == 0) {
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
    fVisible = value;

}

void jwm::WindowMac::reconfigure() {
    std::lock_guard<std::mutex> lock(fDisplayLinkMutex);
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
    // if (!CVDisplayLinkIsRunning(fDisplayLink)) {
    if (fVisible && !fDisplayLinkRunning) {
        std::lock_guard<std::mutex> lock(fDisplayLinkMutex);
        if (fVisible && !fDisplayLinkRunning) {
            CVDisplayLinkStart(fDisplayLink);
            fDisplayLinkRunning = true;
        }
    }
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
    NSString* title = jwm::nsStringFromJava(env, titleStr);
    instance->fNSWindow.title = title;
    [instance->fNSWindow setTitleVisibility:NSWindowTitleVisible];
    [title release];
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowMac__1nSetTitleVisible
  (JNIEnv* env, jobject obj, jboolean value) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    [instance->fNSWindow setTitleVisibility:value ? NSWindowTitleVisible : NSWindowTitleHidden];
}

#if __MAC_OS_X_VERSION_MAX_ALLOWED >= 110000
extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowMac__1nSetSubtitle
  (JNIEnv* env, jobject obj, jstring subtitleStr) {
    if (floor(NSAppKitVersionNumber) <= NSAppKitVersionNumber10_15) {
        jwm::classes::Throwable::throwRuntimeException(env, "WindowMac::setSubtitle is only available on macOS 11+");
        return;
    }
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    jsize len = env->GetStringLength(subtitleStr);
    const jchar* chars = env->GetStringCritical(subtitleStr, nullptr);
    NSString* subtitle = [[NSString alloc] initWithCharacters:chars length:len];
    env->ReleaseStringCritical(subtitleStr, chars);
    instance->fNSWindow.subtitle = subtitle;
    [instance->fNSWindow setTitleVisibility:NSWindowTitleVisible];
    [subtitle release];
}
#endif

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowMac__1nSetRepresentedFilename
  (JNIEnv* env, jobject obj, jstring filenameStr) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    jsize len = env->GetStringLength(filenameStr);
    const jchar* chars = env->GetStringCritical(filenameStr, nullptr);
    NSString* filename = [[NSString alloc] initWithCharacters:chars length:len];
    env->ReleaseStringCritical(filenameStr, chars);
    [instance->fNSWindow setRepresentedFilename:filename];
    [filename release];
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowMac__1nSetIcon
  (JNIEnv* env, jobject obj, jstring pathStr) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    jsize len = env->GetStringLength(pathStr);
    const jchar* chars = env->GetStringCritical(pathStr, nullptr);
    NSString* path = [[NSString alloc] initWithCharacters:chars length:len];
    env->ReleaseStringCritical(pathStr, chars);

    dispatch_async(dispatch_get_main_queue(), ^{
        NSImage* image = [[NSImage alloc] initByReferencingFile:path];
        [path release];
        NSApplication* app = [NSApplication sharedApplication];
        app.applicationIconImage = image;
        [image release];
    });
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

#if __MAC_OS_X_VERSION_MAX_ALLOWED >= 110000
extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowMac__1nSetTitlebarStyle
  (JNIEnv* env, jobject obj, jint titlebarStyle) {
    if (floor(NSAppKitVersionNumber) <= NSAppKitVersionNumber10_15) {
        jwm::classes::Throwable::throwRuntimeException(env, "WindowMac::setSubtitle is only available on macOS 11+");
        return;
    }

    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    NSWindow* nsWindow = instance->fNSWindow;

    NSToolbar* toolbar = nullptr;
    NSWindowToolbarStyle toolbarStyle = NSWindowToolbarStyleAutomatic;
    switch (static_cast<jwm::WindowMacTitlebarStyle>(titlebarStyle)) {
        case jwm::WindowMacTitlebarStyle::DEFAULT:
            // Just set the toolbar to null
            break;
        case jwm::WindowMacTitlebarStyle::UNIFIED:
            toolbar = [[NSToolbar alloc] init];
            toolbarStyle = NSWindowToolbarStyleUnified;
            break;
        case jwm::WindowMacTitlebarStyle::UNIFIED_COMPACT:
            toolbar = [[NSToolbar alloc] init];
            toolbarStyle = NSWindowToolbarStyleUnifiedCompact;
            break;
    }
    [nsWindow setToolbar:toolbar];
    [nsWindow setToolbarStyle:toolbarStyle];
    [toolbar release];
}
#endif

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowMac__1nSetTrafficLightPosition
  (JNIEnv* env, jobject obj, jint left, jint top) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    NSWindow* nsWindow = instance->fNSWindow;

    if (([nsWindow styleMask] & NSWindowStyleMaskTitled) == 0) {
        // Will fail due to missing buttons.
        return;
    }

    // Based on https://www.codetd.com/en/article/6488220
    NSButton *close = [nsWindow standardWindowButton:NSWindowCloseButton];
    NSButton *miniaturize = [nsWindow standardWindowButton:NSWindowMiniaturizeButton];
    NSButton *zoom = [nsWindow standardWindowButton:NSWindowZoomButton];
    NSView *titlebarView = close.superview;

    NSArray* windowButtons = @[close, miniaturize, zoom];
    CGFloat spaceBetween = miniaturize.frame.origin.x - close.frame.origin.x;
    for (NSUInteger i = 0; i < windowButtons.count; i++) {
        NSButton* button = windowButtons[i];

        button.translatesAutoresizingMaskIntoConstraints = NO;
        [titlebarView addConstraints:@[
            [NSLayoutConstraint constraintWithItem:button
                                attribute:NSLayoutAttributeTop
                                relatedBy:NSLayoutRelationEqual
                                toItem:titlebarView
                                attribute:NSLayoutAttributeTop
                                multiplier:1
                                constant:top],
            [NSLayoutConstraint constraintWithItem:button
                                attribute:NSLayoutAttributeLeft
                                relatedBy:NSLayoutRelationEqual
                                toItem:titlebarView
                                attribute:NSLayoutAttributeLeft
                                multiplier:1
                                constant:left + (spaceBetween * i)]
        ]];
    }
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowMac__1nSetTrafficLightsVisible
  (JNIEnv* env, jobject obj, jboolean value) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    NSWindow* nsWindow = instance->fNSWindow;

    NSButton *close = [nsWindow standardWindowButton:NSWindowCloseButton];
    if (close != nullptr) close.hidden = !value;
    NSButton *miniaturize = [nsWindow standardWindowButton:NSWindowMiniaturizeButton];
    if (miniaturize != nullptr) miniaturize.hidden = !value;
    NSButton *zoom = [nsWindow standardWindowButton:NSWindowZoomButton];
    if (zoom != nullptr) zoom.hidden = !value;
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowMac__1nSetMouseCursor
  (JNIEnv* env, jobject obj, jint cursorIdx) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    NSCursor* cursor = [jwm::kCursorCache objectAtIndex:cursorIdx];
    instance->fCursor = cursor;
    [cursor set];
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowMac__1nHideMouseCursorUntilMoved
  (JNIEnv* env, jobject obj, jboolean value) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    [NSCursor setHiddenUntilMouseMoves:value];
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowMac__1nLockMouseCursor
  (JNIEnv* env, jobject obj, jboolean value) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    if (value) {
        CGAssociateMouseAndMouseCursorPosition(NO);
        [NSCursor hide];
    } else {
        CGAssociateMouseAndMouseCursorPosition(YES);
        [NSCursor unhide];
    }
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

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowMac__1nSetFullScreen
  (JNIEnv* env, jobject obj, jboolean value) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    NSWindow* nsWindow = instance->fNSWindow;
    bool isFullScreen = ([nsWindow styleMask] & NSWindowStyleMaskFullScreen) == NSWindowStyleMaskFullScreen;
    if (value && !isFullScreen) {
        [nsWindow toggleFullScreen:nil];
    } else if (!value && isFullScreen) {
        [nsWindow toggleFullScreen:nil];
    }
}

extern "C" JNIEXPORT jboolean JNICALL Java_io_github_humbleui_jwm_WindowMac__1nIsFullScreen
  (JNIEnv* env, jobject obj, jboolean value) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    NSWindow* nsWindow = instance->fNSWindow;
    return ([nsWindow styleMask] & NSWindowStyleMaskFullScreen) == NSWindowStyleMaskFullScreen;
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowMac__1nFocus
  (JNIEnv* env, jobject obj) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    NSWindow* nsWindow = instance->fNSWindow;
    [nsWindow makeKeyAndOrderFront:nil];
}

extern "C" JNIEXPORT bool JNICALL Java_io_github_humbleui_jwm_WindowMac__1nIsFront
  (JNIEnv* env, jobject obj) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    NSWindow* nsWindow = instance->fNSWindow;
    return [nsWindow isMainWindow];
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowMac__1nBringToFront
  (JNIEnv* env, jobject obj) {
    [NSApp activateIgnoringOtherApps:YES];
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

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowMac__1nSetProgressBar
  (JNIEnv* env, jobject obj, jfloat value) {
    // Based on
    // https://github.com/electron/electron/blob/3768a7b25f3db505a25582706de58e7f81121565/shell/browser/native_window_mac.mm#L1268
    // https://github.com/electron/electron/blob/3768a7b25f3db505a25582706de58e7f81121565/LICENSE

    NSDockTile* dockTile = [NSApp dockTile];

    // Check if there is an existing progress indicator
    bool firstTime = !dockTile.contentView || // No content view at all
                     [[dockTile.contentView subviews] count] == 0 || // Content view contains no children
                     ![[[dockTile.contentView subviews] lastObject] isKindOfClass:[NSProgressIndicator class]]; // Child is not a progress indicator

    // Create progress indicator the first time
    if (firstTime) {
      NSImageView* imageView = [[[NSImageView alloc] init] autorelease];
      [imageView setImage:[NSApp applicationIconImage]];
      [dockTile setContentView:imageView];

      NSRect frame = NSMakeRect(0.0f, 0.0f, dockTile.size.width, 15.0);
      NSProgressIndicator* progressIndicator =
          [[[DockTileProgressBar alloc] initWithFrame:frame] autorelease];
      [progressIndicator setStyle:NSProgressIndicatorStyleBar];
      [progressIndicator setIndeterminate:NO];
      [progressIndicator setBezeled:YES];
      [progressIndicator setMinValue:0];
      [progressIndicator setMaxValue:1];
      [progressIndicator setHidden:NO];
      [dockTile.contentView addSubview:progressIndicator];
    }

    // Otherwise update the existing progress indicator
    NSProgressIndicator* progressIndicator =
        static_cast<NSProgressIndicator*>([[[dockTile contentView] subviews] lastObject]);
    if (value < 0) {
      [progressIndicator setHidden:YES];
    } else if (value > 1) {
      [progressIndicator setHidden:NO];
      [progressIndicator setIndeterminate:YES];
      [progressIndicator setDoubleValue:1];
    } else {
      [progressIndicator setHidden:NO];
      [progressIndicator setDoubleValue:value];
    }

    [dockTile display];
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowMac__1nClose
  (JNIEnv* env, jobject obj) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    instance->close();
}
