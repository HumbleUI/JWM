#pragma once
#include <jni.h>

namespace jwm {
    template <typename T>
    class AutoLocal {
    public:
        AutoLocal(JNIEnv* env, T ref): fEnv(env), fRef(ref) {
        }

        AutoLocal(const AutoLocal&) = delete;
        AutoLocal(AutoLocal&&) = default;
        AutoLocal& operator=(AutoLocal const&) = delete;

        ~AutoLocal() {
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

    namespace classes {
        namespace Throwable {
            extern jmethodID kPrintStackTrace;
            bool exceptionThrown(JNIEnv* env);
        }

        namespace Window {
            extern jmethodID kOnEvent;
            void onEvent(JNIEnv* env, jobject window, jobject event);
        }

        namespace CloseEvent {
            extern jclass kCls;
            extern jmethodID kCtor;
            jobject make(JNIEnv* env);
        }

        namespace ResizeEvent {
            extern jclass kCls;
            extern jmethodID kCtor;
            jobject make(JNIEnv* env, jint width, jint height);
        }
    }
}
