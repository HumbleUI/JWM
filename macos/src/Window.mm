#include <iostream>
#include <jni.h>
#include "Library.hh"
#include <memory>
#include "Window.hh"

@interface WindowDelegate : NSObject<NSWindowDelegate>

- (WindowDelegate*)initWithWindow:(jwm::Window*)initWindow;

@end

@interface MainView : NSView

- (MainView*)initWithWindow:(jwm::Window*)initWindow;

@end


// jwm::Window

bool jwm::Window::init() {
    // Create a delegate to track certain events
    WindowDelegate* delegate = [[WindowDelegate alloc] initWithWindow:this];
    if (nil == delegate) {
        return false;
    }

    // Create Cocoa window
    constexpr int initialWidth = 800;
    constexpr int initialHeight = 600;
    NSRect windowRect = NSMakeRect(100, 100, initialWidth, initialHeight);

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

    // fWindowNumber = fNSWindow.windowNumber;
    // gWindowMap.add(this);
    return true;
}

float jwm::Window::scaleFactor() const {
    NSScreen* screen = fNSWindow.contentView.window.screen ?: [NSScreen mainScreen];
    return screen.backingScaleFactor;
}

void jwm::Window::onEvent(jobject event) {
    jwm::classes::Window::onEvent(fEnv, fJavaWindow, event);
}


// JNI

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_Window__1nCreate
  (JNIEnv* env, jclass jclass, jobject javaWindow) {
    std::unique_ptr<jwm::Window> instance(new jwm::Window(env, javaWindow));
    if (instance->init())
      return reinterpret_cast<jlong>(instance.release());
    else
      return 0;
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_Window__1nClose
  (JNIEnv* env, jclass jclass, jlong ptr) {
    jwm::Window* instance = reinterpret_cast<jwm::Window*>(static_cast<uintptr_t>(ptr));
    [instance->fNSWindow close];
    delete instance;
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_Window__1nShow
  (JNIEnv* env, jclass jclass, jlong ptr) {
    jwm::Window* instance = reinterpret_cast<jwm::Window*>(static_cast<uintptr_t>(ptr));
    [instance->fNSWindow orderFront:nil];
    [NSApp activateIgnoringOtherApps:YES];
    [instance->fNSWindow makeKeyAndOrderFront:NSApp];
}

// WindowDelegate

@implementation WindowDelegate {
    jwm::Window* fWindow;
}

- (WindowDelegate*)initWithWindow:(jwm::Window*)initWindow {
    fWindow = initWindow;
    return self;
}

- (void)windowDidResize:(NSNotification *)notification {
    NSView* view = fWindow->fNSWindow.contentView;
    CGFloat scale = fWindow->scaleFactor();

    jwm::AutoLocal<jobject> event(fWindow->fEnv, jwm::classes::ResizeEvent::make(fWindow->fEnv, view.bounds.size.width * scale, view.bounds.size.height * scale));
    fWindow->onEvent(event.get());
    // fWindow->inval();
}

- (BOOL)windowShouldClose:(NSWindow*)sender {
    // fWindow->closeWindow();
    jwm::AutoLocal<jobject> event(fWindow->fEnv, jwm::classes::CloseEvent::make(fWindow->fEnv));
    fWindow->onEvent(event.get());
    return FALSE;
}

@end

// MainView

namespace jwm {
enum class ModifierKey {
    kNone       = 0,
    kShift      = 1 << 0,
    kControl    = 1 << 1,
    kOption     = 1 << 2,   // same as ALT
    kCommand    = 1 << 3,
    kFirstPress = 1 << 4,
};
}

@implementation MainView {
    jwm::Window* fWindow;
    // A TrackingArea prevents us from capturing events outside the view
    NSTrackingArea* fTrackingArea;
    // We keep track of the state of the modifier keys on each event in order to synthesize
    // key-up/down events for each modifier.
    jwm::ModifierKey fLastModifiers;
}

- (MainView*)initWithWindow:(jwm::Window*)initWindow {
    self = [super init];

    fWindow = initWindow;
    fTrackingArea = nil;

    [self updateTrackingAreas];

    return self;
}

- (void)dealloc
{
    [fTrackingArea release];
    [super dealloc];
}

- (BOOL)isOpaque {
    return YES;
}

- (BOOL)canBecomeKeyView {
    return YES;
}

- (BOOL)acceptsFirstResponder {
    return YES;
}

- (void)updateTrackingAreas {
    if (fTrackingArea != nil) {
        [self removeTrackingArea:fTrackingArea];
        [fTrackingArea release];
    }

    const NSTrackingAreaOptions options = NSTrackingMouseEnteredAndExited |
                                          NSTrackingActiveInKeyWindow |
                                          NSTrackingEnabledDuringMouseDrag |
                                          NSTrackingCursorUpdate |
                                          NSTrackingInVisibleRect |
                                          NSTrackingAssumeInside;

    fTrackingArea = [[NSTrackingArea alloc] initWithRect:[self bounds]
                                                 options:options
                                                   owner:self
                                                userInfo:nil];

    [self addTrackingArea:fTrackingArea];
    [super updateTrackingAreas];
}

@end