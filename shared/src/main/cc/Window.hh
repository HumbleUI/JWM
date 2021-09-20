#pragma once
#include <jni.h>
#include "impl/RefCounted.hh"

namespace jwm {
    class Window: public RefCounted {
    public:
        Window(JNIEnv* env): fEnv(env) {
        }

        virtual ~Window();

        void dispatch(jobject event);

        JNIEnv* fEnv = nullptr;
        jobject fEventListener = nullptr;
        jobject fTextInputClient = nullptr;
    };
}