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

- (void)windowDidResize:(NSNotification *)notification {
    NSView* view = fWindow->fNSWindow.contentView;
    CGFloat scale = fWindow->getScale();

    jwm::JNILocal<jobject> eventResize(fWindow->fEnv, jwm::classes::EventResize::make(fWindow->fEnv, view.bounds.size.width * scale, view.bounds.size.height * scale));
    fWindow->dispatch(eventResize.get());
}

- (void)windowDidChangeScreen:(NSNotification*)notification {
    NSWindow* window = fWindow->fNSWindow;
    CGDirectDisplayID displayID = (CGDirectDisplayID)[[[[window screen] deviceDescription] objectForKey:@"NSScreenNumber"] intValue];
    fWindow->reconfigure();
    fWindow->dispatch(jwm::classes::EventReconfigure::kInstance);
}

- (BOOL)windowShouldClose:(NSWindow*)sender {
    std::cout << "windowShouldClose" << std::endl;
    fWindow->dispatch(jwm::classes::EventClose::kInstance);
    return FALSE;
}

@end
