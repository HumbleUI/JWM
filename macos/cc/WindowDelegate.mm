#include <iostream>
#include <jni.h>
#include "impl/JNILocal.hh"
#include "impl/Library.hh"
#include "Layer.hh"
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
    CGFloat scale = fWindow->scaleFactor();

    fWindow->fLayer->resize();

    jwm::JNILocal<jobject> eventResize(fWindow->fEnv, jwm::classes::EventResize::make(fWindow->fEnv, view.bounds.size.width * scale, view.bounds.size.height * scale));
    fWindow->onEvent(eventResize.get());

    fWindow->onEvent(jwm::classes::EventPaint::kInstance);
}

- (void)windowDidChangeScreen:(NSNotification*)notification {
    NSWindow* window = fWindow->fNSWindow;
    CGDirectDisplayID displayID = (CGDirectDisplayID)[[[[window screen] deviceDescription] objectForKey:@"NSScreenNumber"] intValue];
    fWindow->invalidate();
    fWindow->fLayer->invalidate();
    fWindow->fLayer->resize();
}

- (BOOL)windowShouldClose:(NSWindow*)sender {
    jwm::JNILocal<jobject> event(fWindow->fEnv, jwm::classes::EventClose::make(fWindow->fEnv));
    fWindow->onEvent(event.get());
    return FALSE;
}

@end
