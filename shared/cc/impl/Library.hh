#pragma once
#include <jni.h>
#include <cstdint>
#include "Key.hh"
#include "KeyLocation.hh"
#include "MouseButton.hh"
#include "Types.hh"
#include "Window.hh"

namespace jwm {
    struct UIRect {
        int32_t fLeft;
        int32_t fTop;
        int32_t fRight;
        int32_t fBottom;

        int32_t getWidth() const { return fRight - fLeft; }
        int32_t getHeight() const { return fBottom - fTop; }

        bool isPointInside(int32_t x, int32_t y) const {
            return x >= fLeft   &&
                   y >= fTop    &&
                   x <  fRight  &&
                   y <  fBottom;
        }

        static UIRect makeXYWH(int32_t left, int32_t top, int32_t width, int32_t height) {
            return { left, top, left + width, top + height };
        }
    };

    namespace classes {
        namespace Throwable {
            extern jclass kClsRuntimeException;
            extern jclass kClsLayerNotSupportedException;
            extern jmethodID kPrintStackTrace;
            bool exceptionThrown(JNIEnv* env);
            void throwRuntimeException(JNIEnv* env, const char* message);
            void throwLayerNotSupportedException(JNIEnv* env, const char* message);
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

        namespace EventWindowScreenChange {
            extern jobject kInstance;
        }

        namespace EventFrame {
            extern jobject kInstance;
        }

        namespace EventKey {
            extern jclass kCls;
            extern jmethodID kCtor;
            jobject make(JNIEnv* env, Key keyCode, jboolean isPressed, jint jmodifiers, KeyLocation location = KeyLocation::DEFAULT);
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

        namespace EventMouseScroll {
            extern jclass kCls;
            extern jmethodID kCtor;
            jobject make(JNIEnv* env, jfloat deltaX, jfloat deltaY, jfloat deltaChars, jfloat deltaLines, jfloat deltaPages, jint modifiers);
        }

        namespace EventTextInput {
            extern jclass kCls;
            extern jmethodID kCtor;
            jobject make(JNIEnv* env, jstring text);
        }

        namespace EventTextInputMarked {
            extern jclass kCls;
            extern jmethodID kCtor;
            jobject make(JNIEnv* env, jstring text, jint selectionStart, jint selectionEnd);
        }

        namespace EventWindowCloseRequest {
            extern jobject kInstance;
        }

        namespace EventWindowMaximize {
            extern jobject kInstance;
        }

        namespace EventWindowMinimize {
            extern jobject kInstance;
        }

        namespace EventWindowMove {
            extern jclass kCls;
            extern jmethodID kCtor;
            jobject make(JNIEnv* env, jint left, jint top);
        }

        namespace EventWindowResize {
            extern jclass kCls;
            extern jmethodID kCtor;
            jobject make(JNIEnv* env, jint windowWidth, jint windowHeight, jint contentWidth, jint contentHeight);
        }

        namespace EventWindowRestore {
            extern jobject kInstance;
        }
        
        namespace Screen {
            extern jclass kCls;
            extern jmethodID kCtor;
            jobject make(JNIEnv* env, jlong id, jboolean isPrimary, jwm::UIRect bounds, jwm::UIRect workArea, jfloat scale);
        }

        namespace TextInputClient {
            extern jclass kCls;
            extern jmethodID kGetRectForMarkedRange;
            jwm::UIRect getRectForMarkedRange(JNIEnv* env, jobject client, jint selectionStart, jint selectionEnd);
        }

        namespace UIRect {
            extern jclass kCls;
            extern jmethodID kCtor;
            extern jfieldID kLeft;
            extern jfieldID kTop;
            extern jfieldID kRight;
            extern jfieldID kBottom;
            jwm::UIRect fromJava(JNIEnv* env, jobject uirect);
            jobject toJava(JNIEnv* env, const struct UIRect& rect);
            jobject toJavaXYWH(JNIEnv* env, jint left, jint top, jint width, jint height);
        }
    }
}
