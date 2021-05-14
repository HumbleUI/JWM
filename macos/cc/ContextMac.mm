#include "ContextMac.hh"

namespace jwm {

static CVReturn displayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* _now, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* ctx) {
    ContextMac* self = (ContextMac*) ctx;
    if (self->fDrawMutex.try_lock()) {
        self->ref();
        dispatch_async(dispatch_get_main_queue(), ^{
            if (self->fWindow) {
                self->fWindow->onEvent(jwm::classes::EventPaint::kInstance);
                self->fDrawMutex.unlock();
            }
            self->unref();
        });
    }
    return kCVReturnSuccess;
}

void ContextMac::attach(Window* window) {
    WindowMac* windowMac = reinterpret_cast<WindowMac*>(window);
    fWindow = windowMac;
    fMainView = [windowMac->fNSWindow contentView];
}

void ContextMac::reinit() {
    if (!fDisplayLink) {
        CVDisplayLinkCreateWithActiveCGDisplays(&fDisplayLink);
        CVDisplayLinkSetOutputCallback(fDisplayLink, &displayLinkCallback, this);
        CVDisplayLinkStart(fDisplayLink);
    }

    NSWindow* window = fMainView.window;
    CGDirectDisplayID currentDisplay = (CGDirectDisplayID)[[[[window screen] deviceDescription] objectForKey:@"NSScreenNumber"] intValue];
    CGDirectDisplayID oldDisplay = CVDisplayLinkGetCurrentCGDisplay(fDisplayLink);
    if (currentDisplay != oldDisplay)
        CVDisplayLinkSetCurrentCGDisplay(fDisplayLink, currentDisplay);
}

} // namespace jwm
