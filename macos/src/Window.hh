#pragma once
#import <Cocoa/Cocoa.h>
#include <jni.h>

namespace jwm {
    class Window {
    public:
        Window(JNIEnv* env, jobject javaWindow): fEnv(env) {
            fJavaWindow = env->NewGlobalRef(javaWindow);
        }

        ~Window() {
            fEnv->DeleteGlobalRef(fJavaWindow);
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
        jobject fJavaWindow;
    };
}