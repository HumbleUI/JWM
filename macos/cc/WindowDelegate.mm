#include <iostream>
#include <jni.h>
#include "impl/JNILocal.hh"
#include "impl/Library.hh"
#include "WindowMac.hh"
#include "WindowDelegate.hh"

@implementation WindowDelegate {
    jwm::WindowMac* fWindow;
}

- (WindowDelegate*)initWithWindow:(jwm::WindowMac*)initWindow {
    fWindow = initWindow;
    return self;
}

- (void)windowDidMove:(NSNotification *)notification {
    NSWindow* window = fWindow->fNSWindow;
    auto screen = window.screen ?: [NSScreen mainScreen];
    auto left = window.frame.origin.x;
    auto top = screen.frame.size.height - window.frame.origin.y - window.frame.size.height;

    CGFloat scale = fWindow->getScale();
    jwm::JNILocal<jobject> event(fWindow->fEnv, jwm::classes::EventWindowMove::make(fWindow->fEnv, left * scale, top * scale));
    fWindow->dispatch(event.get());
}

- (void)windowDidResize:(NSNotification *)notification {
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
    fWindow->dispatch(jwm::classes::EventEnvironmentChange::kInstance);
}

- (BOOL)windowShouldClose:(NSWindow*)sender {
    std::cout << "windowShouldClose" << std::endl;
    fWindow->dispatch(jwm::classes::EventWindowCloseRequest::kInstance);
    return FALSE;
}

@end
