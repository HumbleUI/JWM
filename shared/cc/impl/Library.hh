#pragma once
#include <jni.h>
#include <cstdint>
#include "Key.hh"
#include "MouseButton.hh"
#include "Types.hh"

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

        namespace Clipboard {
            extern jclass kCls;
            extern jmethodID kRegisterPredefinedFormat;
            jobject registerFormat(JNIEnv* env, jstring formatId);
        }

        namespace ClipboardEntry {
            extern jclass kCls;
            extern jmethodID kMake;
            extern jfieldID kFormat;
            extern jfieldID kData;
            jobject make(JNIEnv* env, jobject format, jbyteArray data);
            jobject getFormat(JNIEnv* env, jobject clipboardEntry);
            jbyteArray getData(JNIEnv* env, jobject clipboardEntry);
        }

        namespace ClipboardFormat {
            extern jclass kCls;
            extern jfieldID kFormatId;
            jstring getFormatId(JNIEnv* env, jobject clipboardFormat);
        }

        namespace EventClose {
            extern jobject kInstance;
        }

        namespace EventFrame {
            extern jobject kInstance;
        }

        namespace EventKeyboard {
            extern jclass kCls;
            extern jmethodID kCtor;
            jobject make(JNIEnv* env, Key keyCode, jboolean isPressed, jint jmodifiers);
        }

        namespace EventMouseButton {
            extern jclass kCls;
            extern jmethodID kCtor;
            jobject make(JNIEnv* env, MouseButton mouseButton, jboolean isPressed, jint modifiers);
        }

        namespace EventMouseMove {
            extern jclass kCls;
            extern jmethodID kCtor;
            jobject make(JNIEnv* env, jint x, jint y, jint buttons = 0, jint modifiers = 0);
        }

        namespace EventReconfigure {
            extern jobject kInstance;
        }

        namespace EventResize {
            extern jclass kCls;
            extern jmethodID kCtor;
            jobject make(JNIEnv* env, jint width, jint height);
        }

        namespace EventScroll {
            extern jclass kCls;
            extern jmethodID kCtor;
            jobject make(JNIEnv* env, jfloat dx, jfloat dy, jint modifiers = 0);
        }

        namespace EventTextInput {
            extern jclass kCls;
            extern jmethodID kCtor;
            jobject make(JNIEnv* env, jstring text);
        }

        namespace EventTextInputMarked {
            extern jclass kCls;
            extern jmethodID kCtor;
            jobject make(JNIEnv* env, jstring text, jint selectedFrom, jint selectedTo);
        }

        namespace EventWindowMove {
            extern jclass kCls;
            extern jmethodID kCtor;
            jobject make(JNIEnv* env, jint left, jint top);
        }
        
        namespace Screen {
            extern jclass kCls;
            extern jmethodID kCtor;
            jobject make(JNIEnv* env, jlong id, jint x, jint y, jint width, jint height, jfloat scale, jboolean isPrimary);
        }
    }
}
