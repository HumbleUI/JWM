#include <jni.h>
#include "Library.hh"
#include "Window.hh"
#include "WindowDelegate.hh"

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
