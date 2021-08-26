#pragma once
#import <Cocoa/Cocoa.h>
#import  <CoreVideo/CoreVideo.h>
#include "Window.hh"
#include <jni.h>

namespace jwm {
    extern NSArray* kCursorCache;
    void initCursorCache();

    class WindowMac: public Window {
    public:
        WindowMac(JNIEnv* env): Window(env) {}
        ~WindowMac() override;
        bool init();
        void setVisible(bool value);
        void reconfigure();
        float getScale() const;
        void requestFrame();
        bool isMaximized();
        void close();

        NSWindow* fNSWindow = nullptr;
        NSPoint fLastPosition = {0, 0};
        NSRect fRestoreFrame = NSZeroRect;
        CVDisplayLinkRef fDisplayLink = 0;
        volatile bool fFrameRequested = false;
        volatile bool fFrameScheduled = false;
    };
}