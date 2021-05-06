#pragma once
#include <jni.h>


namespace jwm {
    class Context;
    
    class Window {
    public:
        Window(JNIEnv* env): fEnv(env) {}

        virtual ~Window() = default;

        virtual bool init() = 0;

        virtual float scaleFactor() const = 0;

        void onEvent(jobject event);

        JNIEnv* fEnv = nullptr;
        jobject fEventListener = nullptr;
        Context* fContext = nullptr;
    };

    void deleteWindow(jwm::Window* instance);
}