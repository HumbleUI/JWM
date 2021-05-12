#pragma once

#import  <Cocoa/Cocoa.h>
#import  <CoreVideo/CoreVideo.h>
#include <iostream>
#include <jni.h>
#include "impl/Library.hh"
#include "MainView.hh"
#include "WindowMac.hh"

namespace jwm {

class ContextMac: public Context {
public:
    ContextMac(bool useVsync, bool useDisplayLink): fUseVsync(useVsync), fUseDisplayLink(useDisplayLink) {}
    ~ContextMac();

    void attach(Window* window) override;
    void reinit() override;
    // void resize() override;
    void swapBuffers() override;

    bool              fUseVsync;
    bool              fUseDisplayLink;
    NSView*           fMainView;
    CVDisplayLinkRef  fDisplayLink = 0;
    volatile int      fSwapIntervalsPassed = 0;
    id                fSwapIntervalCond;
};

} // namespace jwm
