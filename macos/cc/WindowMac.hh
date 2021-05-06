#pragma once
#import <Cocoa/Cocoa.h>
#include "Context.hh"
#include <jni.h>

namespace jwm {
    class WindowMac {
    public:
        WindowMac(JNIEnv* env): fEnv(env) {}
        ~WindowMac();
        bool init();
        float scaleFactor() const;
        void onEvent(jobject event);

        NSWindow* fNSWindow = nullptr;
        // NSInteger fNSWindowNumber;
        JNIEnv* fEnv = nullptr;
        jobject fEventListener = nullptr;
        Context* fContext = nullptr;
    };

    void deleteWindowMac(jwm::WindowMac* instance);
}