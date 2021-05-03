#pragma once
#import <Cocoa/Cocoa.h>
#include <jni.h>

namespace jwm {
    class Window {
    public:
        Window(JNIEnv* env): fEnv(env) {
        }

        ~Window() {
            if (fEventListener)
                fEnv->DeleteGlobalRef(fEventListener);
            // this->closeWindow();
        }

        bool init();
        // void setTitle(const char*);
        // void show();
        // void onInval() {}
        float scaleFactor() const;

        // void closeWindow();

        void onEvent(jobject event);

        NSWindow* fNSWindow;
        // NSInteger fNSWindowNumber;
        JNIEnv* fEnv;
        jobject fEventListener = nullptr;
    };
}