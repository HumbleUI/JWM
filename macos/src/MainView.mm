#include <jni.h>
#include "Library.hh"
#include "Window.hh"
#include "MainView.hh"

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

- (void)mouseMoved:(NSEvent *)event {
    NSView* view = fWindow->fNSWindow.contentView;
    CGFloat scale = fWindow->scaleFactor();

    // skui::ModifierKey modifiers = [self updateModifierKeys:event];

    const NSPoint pos = [event locationInWindow];
    const NSRect rect = [view frame];
    jwm::AutoLocal<jobject> eventObj(fWindow->fEnv, jwm::classes::MouseMoveEvent::make(fWindow->fEnv, (jint) (pos.x * scale), (jint) ((rect.size.height - pos.y) * scale)));
    fWindow->onEvent(eventObj.get());
}

@end