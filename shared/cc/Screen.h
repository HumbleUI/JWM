#pragma once
#include <jni.h>
#include "impl/RefCounted.hh"

namespace jwm {

    class Screen: public RefCounted {
    public:
        Screen(JNIEnv* env): fEnv(env) {
        }

        virtual ~Screen() = default;

        JNIEnv* fEnv = nullptr;
    };

}