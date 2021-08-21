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
        bool init();
        void show();
        void reconfigure();
        float getScale() const;
        void requestFrame();
        void close();

        NSWindow* fNSWindow = nullptr;
        NSPoint fLastPosition = {0, 0};
        CVDisplayLinkRef fDisplayLink = 0;
        volatile bool fFrameRequested = false;
        volatile bool fFrameScheduled = false;
    };
}