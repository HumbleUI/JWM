#include "ContextMac.hh"

namespace jwm {

ContextMac::~ContextMac() {
    if (fUseDisplayLink) {
        CVDisplayLinkStop(fDisplayLink);
        CVDisplayLinkRelease(fDisplayLink);
    }
}

static CVReturn displayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* _now, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* ctx) {
    ContextMac* self = (ContextMac*) ctx;
    [self->fSwapIntervalCond lock];
    self->fSwapIntervalsPassed++;
    [self->fSwapIntervalCond signal];
    [self->fSwapIntervalCond unlock];
    return kCVReturnSuccess;
}

void ContextMac::attach(Window* window) {
    WindowMac* windowMac = reinterpret_cast<WindowMac*>(window);
    fMainView = [windowMac->fNSWindow contentView];
}

void ContextMac::reinit() {
    if (fUseDisplayLink && !fDisplayLink) {
        CVDisplayLinkCreateWithActiveCGDisplays(&fDisplayLink);
        CVDisplayLinkSetOutputCallback(fDisplayLink, &displayLinkCallback, this);
        CVDisplayLinkStart(fDisplayLink);
        fSwapIntervalCond = [NSCondition new];
    }

    if (fUseDisplayLink) {
        NSWindow* window = fMainView.window;
        CGDirectDisplayID currentDisplay = (CGDirectDisplayID)[[[[window screen] deviceDescription] objectForKey:@"NSScreenNumber"] intValue];
        CGDirectDisplayID oldDisplay = CVDisplayLinkGetCurrentCGDisplay(fDisplayLink);
        if (currentDisplay != oldDisplay) {
            std::cout << "Jumping displays from " << oldDisplay << " to " << currentDisplay << std::endl;
            CVDisplayLinkSetCurrentCGDisplay(fDisplayLink, currentDisplay);
        }
    }
}

void ContextMac::swapBuffers() {
    if (fUseDisplayLink) {
        [this->fSwapIntervalCond lock];
        do {
            [this->fSwapIntervalCond wait];
        } while (this->fSwapIntervalsPassed == 0);
        this->fSwapIntervalsPassed = 0;
        [this->fSwapIntervalCond unlock];
    }
}

} // namespace jwm
