#include <iostream>
#include <jni.h>
#include "impl/JNILocal.hh"
#include "impl/Library.hh"
#include "WindowMac.hh"
#include "JWMWindowDelegate.hh"
#include "Util.hh"

@implementation JWMWindowDelegate {
    jwm::WindowMac* fWindow;
}

- (JWMWindowDelegate*)initWithWindow:(jwm::WindowMac*)initWindow {
    fWindow = initWindow;
    return self;
}

- (void)windowDidMove:(NSNotification *)notification {
    NSWindow* window = fWindow->fNSWindow;
    NSScreen* screen = [window screen] ?: [NSScreen mainScreen];
    NSRect frame = [fWindow->fNSWindow frame];
    frame.origin.y = screen.frame.size.height - frame.origin.y - frame.size.height;
    if (frame.origin.x != fWindow->fLastPosition.x || frame.origin.y != fWindow->fLastPosition.y) {
        fWindow->fLastPosition = frame.origin;
        NSPoint pos = jwm::nsWindowPosition(fWindow->fNSWindow);
        jwm::JNILocal<jobject> event(fWindow->fEnv, jwm::classes::EventWindowMove::make(fWindow->fEnv, pos.x, pos.y));
        fWindow->dispatch(event.get());
    }
}

- (void)windowDidResize:(NSNotification *)notification {
    [self windowDidMove:notification];
    const NSRect windowFrame = [fWindow->fNSWindow frame];
    const NSRect contentFrame = [fWindow->fNSWindow.contentView frame];
    CGFloat scale = fWindow->getScale();

    jwm::JNILocal<jobject> eventWindowResize(
        fWindow->fEnv,
        jwm::classes::EventWindowResize::make(
            fWindow->fEnv,
            windowFrame.size.width * scale,
            windowFrame.size.height * scale,
            contentFrame.size.width * scale,
            contentFrame.size.height * scale));
    fWindow->dispatch(eventWindowResize.get());
}

- (void)windowDidChangeScreen:(NSNotification*)notification {
    NSWindow* window = fWindow->fNSWindow;
    CGDirectDisplayID displayID = (CGDirectDisplayID)[[[[window screen] deviceDescription] objectForKey:@"NSScreenNumber"] intValue];
    fWindow->reconfigure();
    fWindow->dispatch(jwm::classes::EventWindowScreenChange::kInstance);
}

- (void)windowDidBecomeKey:(NSNotification *)notification {
    fWindow->dispatch(jwm::classes::EventWindowFocusIn::kInstance);
}

- (void)windowDidResignKey:(NSNotification *)notification {
    fWindow->dispatch(jwm::classes::EventWindowFocusOut::kInstance);
}

- (BOOL)windowShouldClose:(NSWindow*)sender {
    fWindow->dispatch(jwm::classes::EventWindowCloseRequest::kInstance);
    return FALSE;
}

@end
