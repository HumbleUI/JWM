#pragma once

#import  <Cocoa/Cocoa.h>
#import  <CoreVideo/CoreVideo.h>
#include <iostream>
#include <jni.h>
#include "impl/Library.hh"
#include "MainView.hh"
#include <mutex>
#include "WindowMac.hh"

namespace jwm {

class ContextMac: public Context {
public:
    ContextMac() = default;
    ~ContextMac() = default;

    void attach(Window* window) override;
    // void detach() override;
    void reinit() override;
    // void resize() override;
    // void swapBuffers() override;

    WindowMac*        fWindow;
    NSView*           fMainView;
    CVDisplayLinkRef  fDisplayLink = 0;
    std::mutex        fDrawMutex;
};

} // namespace jwm
