#pragma once
#include <jni.h>

namespace jwm {
    template <typename T>
    class JNILocal {
    public:
        JNILocal(JNIEnv* env, T ref): fEnv(env), fRef(ref) {
        }

        JNILocal(const JNILocal&) = delete;
        JNILocal(JNILocal&&) = default;
        JNILocal& operator=(JNILocal const&) = delete;

        ~JNILocal() {
            if (fRef)
                fEnv->DeleteLocalRef(fRef);
        }

        T get() {
            return fRef;
        }
    private:
        JNIEnv* fEnv;
        T fRef;
    };
}
