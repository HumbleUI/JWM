#pragma once
#include <jni.h>

namespace jwm {
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
