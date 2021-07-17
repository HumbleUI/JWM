#pragma once
#include <jni.h>
#include <cstdint>
#include "Key.hh"
#include "MouseButton.hh"

namespace jwm {
    namespace classes {
        namespace Throwable {
            extern jmethodID kPrintStackTrace;
            bool exceptionThrown(JNIEnv* env);
        }

        namespace Consumer {
            extern jmethodID kAccept;
            bool accept(JNIEnv* env, jobject consumer, jobject event);
        }

        namespace Runnable {
            extern jmethodID kRun;
            bool run(JNIEnv* env, jobject runnable);
        }

        namespace Native {
            extern jfieldID kPtr;
            uintptr_t fromJava(JNIEnv* env, jobject object);
        }

        namespace EventClose {
            extern jobject kInstance;
        }

        namespace EventFrame {
            extern jobject kInstance;
        }

        namespace EventMouseMove {
            extern jclass kCls;
            extern jmethodID kCtor;
            jobject make(JNIEnv* env, jint x, jint y);
        }

        namespace EventMouseButton {
            extern jclass kCls;
            extern jmethodID kCtor;
            jobject make(JNIEnv* env, MouseButton mouseButton, bool isPressed, int modifiers);
        }

        namespace EventKeyboard {
            extern jclass kCls;
            extern jmethodID kCtor;
            jobject make(JNIEnv* env, Key keyCode, jboolean isPressed, int modifiers);
        }

        namespace EventReconfigure {
            extern jobject kInstance;
        }

        namespace EventResize {
            extern jclass kCls;
            extern jmethodID kCtor;
            jobject make(JNIEnv* env, jint width, jint height);
        }

        namespace Screen {
            extern jclass kCls;
            extern jmethodID kCtor;
            jobject make(JNIEnv* env, jlong id, jint x, jint y, jint width, jint height, jfloat scale, jboolean isPrimary);
        }
    }
}
