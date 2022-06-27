#pragma once
#include <atomic>
#import <Cocoa/Cocoa.h>
#import  <CoreVideo/CoreVideo.h>
#include <mutex>
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
        NSCursor* fCursor = nullptr;
        CVDisplayLinkRef fDisplayLink = 0;
        std::atomic_bool fVisible {false};
        std::atomic_bool fFrameRequested {false};
        std::atomic_bool fFrameScheduled {false};
        std::atomic_bool fDisplayLinkRunning {false};
        std::mutex fDisplayLinkMutex;
    };
}