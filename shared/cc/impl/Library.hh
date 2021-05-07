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

    enum class ModifierKey {
        kNone       = 0,
        kShift      = 1 << 0,
        kControl    = 1 << 1,
        kOption     = 1 << 2,   // same as ALT
        kCommand    = 1 << 3,
        kFirstPress = 1 << 4,
    };

    namespace classes {
        namespace Throwable {
            extern jmethodID kPrintStackTrace;
            bool exceptionThrown(JNIEnv* env);
        }

        namespace Consumer {
            extern jmethodID kAccept;
            void accept(JNIEnv* env, jobject consumer, jobject event);
        }

        namespace Runnable {
            extern jmethodID kRun;
            void run(JNIEnv* env, jobject object);
        }

        namespace Native {
            extern jfieldID kPtr;
            uintptr_t fromJava(JNIEnv* env, jobject object);
        }

        namespace EventClose {
            extern jclass kCls;
            extern jmethodID kCtor;
            jobject make(JNIEnv* env);
        }

        namespace EventMouseMove {
            extern jclass kCls;
            extern jmethodID kCtor;
            jobject make(JNIEnv* env, jint x, jint y);
        }

        namespace EventKeyboard {
            extern jclass kCls;
            extern jmethodID kCtor;
            jobject make(JNIEnv* env, jint keyCode, jboolean isPressed);
        }

        namespace EventPaint {
            extern jobject kInstance;
        }

        namespace EventResize {
            extern jclass kCls;
            extern jmethodID kCtor;
            jobject make(JNIEnv* env, jint width, jint height);
        }
    }
}
