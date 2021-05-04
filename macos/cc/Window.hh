#pragma once
#import <Cocoa/Cocoa.h>
#include <jni.h>

namespace jwm {
    class Window {
    public:
        Window(JNIEnv* env): fEnv(env) {}
        ~Window();
        bool init();
        float scaleFactor() const;
        void onEvent(jobject event);

        NSWindow* fNSWindow = nullptr;
        // NSInteger fNSWindowNumber;
        JNIEnv* fEnv = nullptr;
        jobject fEventListener = nullptr;
    };

    void deleteWindow(jwm::Window* instance);
}