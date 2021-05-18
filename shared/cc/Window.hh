#pragma once
#include <jni.h>
#include "impl/RefCounted.hh"

namespace jwm {
    class Layer;
    
    class Window: public RefCounted {
    public:
        Window(JNIEnv* env): fEnv(env) {
        }

        virtual ~Window();

        virtual bool init() = 0;

        virtual void invalidate() = 0;

        virtual float scaleFactor() const = 0;

        void onEvent(jobject event);

        JNIEnv* fEnv = nullptr;
        jobject fEventListener = nullptr;
        Layer* fLayer = nullptr;
    };
}