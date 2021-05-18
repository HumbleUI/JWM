#pragma once
#import <Cocoa/Cocoa.h>
#import  <CoreVideo/CoreVideo.h>
#include "Window.hh"
#include <jni.h>

namespace jwm {
    class WindowMac: public Window {
    public:
        WindowMac(JNIEnv* env): Window(env) {}
        ~WindowMac() override;
        bool init() override;
        void invalidate() override;
        float scaleFactor() const override;

        NSWindow* fNSWindow = nullptr;
        CVDisplayLinkRef fDisplayLink = 0;
        volatile bool fNeedRedraw = true;
    };
}