#include <iostream>
#include <jni.h>
#include "Library.hh"
#include <cassert>
#include "Key.hh"
#include "MouseButton.hh"
#include "JNILocal.hh"
#include "StringUTF16.hh"

namespace jwm {
    namespace classes {
        namespace Throwable {
            jclass kClsRuntimeException;
            jclass kClsLayerNotSupportedException;
            jmethodID kPrintStackTrace;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("java/lang/Throwable");
                kPrintStackTrace = env->GetMethodID(cls, "printStackTrace", "()V");

                cls = env->FindClass("java/lang/RuntimeException");
                Throwable::exceptionThrown(env);
                kClsRuntimeException = static_cast<jclass>(env->NewGlobalRef(cls));
                assert(kClsRuntimeException);

                cls = env->FindClass("io/github/humbleui/jwm/LayerNotSupportedException");
                Throwable::exceptionThrown(env);
                kClsLayerNotSupportedException = static_cast<jclass>(env->NewGlobalRef(cls));
                assert(kClsLayerNotSupportedException);
            }

            bool exceptionThrown(JNIEnv* env) {
                if (env->ExceptionCheck()) {
                    jwm::JNILocal<jthrowable> th(env, env->ExceptionOccurred());
                    env->CallVoidMethod(th.get(), kPrintStackTrace);
                    if (env->ExceptionCheck())
                        env->DeleteLocalRef(env->ExceptionOccurred()); // ignore
                    return true;
                } else
                    return false;
            }

            void throwRuntimeException(JNIEnv* env, const char* message) {
                env->ThrowNew(kClsRuntimeException, message);
            }

            void throwLayerNotSupportedException(JNIEnv* env, const char* message) {
                env->ThrowNew(kClsLayerNotSupportedException, message);
            }
        }

        namespace Consumer {
            jmethodID kAccept;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("java/util/function/Consumer");
                Throwable::exceptionThrown(env);
                kAccept = env->GetMethodID(cls, "accept", "(Ljava/lang/Object;)V");
                Throwable::exceptionThrown(env);
            }

            bool accept(JNIEnv* env, jobject consumer, jobject event) {
                env->CallVoidMethod(consumer, kAccept, event);
                return Throwable::exceptionThrown(env);
            }
        }

        namespace Runnable {
            jmethodID kRun;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("java/lang/Runnable");
                Throwable::exceptionThrown(env);
                kRun = env->GetMethodID(cls, "run", "()V");
                Throwable::exceptionThrown(env);
            }

            bool run(JNIEnv* env, jobject runnable) {
                env->CallVoidMethod(runnable, kRun);
                return Throwable::exceptionThrown(env);
            }
        }

        namespace Native {
            jfieldID kPtr;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("io/github/humbleui/jwm/impl/Native");
                Throwable::exceptionThrown(env);
                kPtr = env->GetFieldID(cls, "_ptr", "J");
                Throwable::exceptionThrown(env);
            }

            uintptr_t fromJava(JNIEnv* env, jobject object) {
                jlong ptr = env->GetLongField(object, kPtr);
                return static_cast<uintptr_t>(ptr);
            }
        }

        namespace Clipboard {
            jclass kCls;
            jmethodID kRegisterPredefinedFormat;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("io/github/humbleui/jwm/Clipboard");
                Throwable::exceptionThrown(env);
                kCls = static_cast<jclass>(env->NewGlobalRef(cls));
                assert(kCls);

                // public static ClipboardFormat _registerPredefinedFormat(String formatId)
                kRegisterPredefinedFormat = env->GetStaticMethodID(kCls,
                "_registerPredefinedFormat",
                "(Ljava/lang/String;)Lio/github/humbleui/jwm/ClipboardFormat;"
                );
                Throwable::exceptionThrown(env);
                assert(kRegisterPredefinedFormat);
            }

            jobject registerFormat(JNIEnv* env, jstring formatId) {
                jobject clipboardFormat = env->CallStaticObjectMethod(kCls, kRegisterPredefinedFormat, formatId);
                return Throwable::exceptionThrown(env)? nullptr: clipboardFormat;
            }
        }

        namespace ClipboardEntry {
            jclass kCls;
            jmethodID kMake;
            jfieldID kFormat;
            jfieldID kData;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("io/github/humbleui/jwm/ClipboardEntry");
                Throwable::exceptionThrown(env);
                kCls = static_cast<jclass>(env->NewGlobalRef(cls));
                assert(kCls);

                // public static ClipboardEntry make(ClipboardFormat format, byte[] data)
                kMake = env->GetStaticMethodID(kCls, "make",
                "(Lio/github/humbleui/jwm/ClipboardFormat;[B)"
                    "Lio/github/humbleui/jwm/ClipboardEntry;"
                );
                Throwable::exceptionThrown(env);
                assert(kMake);

                kFormat = env->GetFieldID(kCls, "_format", "Lio/github/humbleui/jwm/ClipboardFormat;");
                Throwable::exceptionThrown(env);
                assert(kFormat);

                kData = env->GetFieldID(kCls, "_data", "[B");
                Throwable::exceptionThrown(env);
                assert(kData);
            }

            jobject make(JNIEnv* env, jobject format, jbyteArray data) {
                jobject clipboardEntry = env->CallStaticObjectMethod(kCls, kMake, format, data);
                return Throwable::exceptionThrown(env)? nullptr: clipboardEntry;
            }

            jobject getFormat(JNIEnv* env, jobject clipboardEntry) {
                jobject format = env->GetObjectField(clipboardEntry, kFormat);
                return Throwable::exceptionThrown(env)? nullptr: format;
            }

            jbyteArray getData(JNIEnv* env, jobject clipboardEntry) {
                jobject data = env->GetObjectField(clipboardEntry, kData);
                return Throwable::exceptionThrown(env)? nullptr: static_cast<jbyteArray>(data);
            }
        }

        namespace ClipboardFormat {
            jclass kCls;
            jfieldID kFormatId;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("io/github/humbleui/jwm/ClipboardFormat");
                Throwable::exceptionThrown(env);
                kCls = static_cast<jclass>(env->NewGlobalRef(cls));
                assert(kCls);

                kFormatId = env->GetFieldID(cls, "_formatId", "Ljava/lang/String;");
                Throwable::exceptionThrown(env);
                assert(kFormatId);
            }

            jstring getFormatId(JNIEnv* env, jobject clipboardFormat) {
                jobject formatId = env->GetObjectField(clipboardFormat, kFormatId);
                return Throwable::exceptionThrown(env)? nullptr: static_cast<jstring>(formatId);
            }
        }

        namespace EventWindowScreenChange {
            jobject kInstance;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("io/github/humbleui/jwm/EventWindowScreenChange");
                Throwable::exceptionThrown(env);
                jfieldID field = env->GetStaticFieldID(cls, "INSTANCE", "Lio/github/humbleui/jwm/EventWindowScreenChange;");
                jobject instance = env->GetStaticObjectField(cls, field);
                kInstance = env->NewGlobalRef(instance);
            }
        }

        namespace EventFrame {
            jobject kInstance;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("io/github/humbleui/jwm/EventFrame");
                Throwable::exceptionThrown(env);
                jfieldID field = env->GetStaticFieldID(cls, "INSTANCE", "Lio/github/humbleui/jwm/EventFrame;");
                jobject instance = env->GetStaticObjectField(cls, field);
                kInstance = env->NewGlobalRef(instance);
            }
        }

        namespace EventKey {
            jclass kCls;
            jmethodID kCtor;
            jobjectArray kKeys;

            void onLoad(JNIEnv* env) {
                // kCls = EventKey
                {
                    jclass cls = env->FindClass("io/github/humbleui/jwm/EventKey");
                    Throwable::exceptionThrown(env);
                    kCls = static_cast<jclass>(env->NewGlobalRef(cls));
                    assert(kCls);
                }

                // kCtor = EventKey::<init>(Key key, boolean isPressed, int modifiers)
                {
                    kCtor = env->GetMethodID(kCls, "<init>", "(IZII)V");
                    Throwable::exceptionThrown(env);
                    assert(kCtor);
                }
            }

            jobject make(JNIEnv* env, Key keyCode, jboolean isPressed, jint modifiers, KeyLocation location) {
                jobject res = env->NewObject(kCls, kCtor, static_cast<int>(keyCode), isPressed, modifiers, static_cast<int>(location));
                return Throwable::exceptionThrown(env) ? nullptr : res;
            }
        }

        namespace EventMouseButton {
            jclass kCls;
            jmethodID kCtor;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("io/github/humbleui/jwm/EventMouseButton");
                Throwable::exceptionThrown(env);
                kCls = static_cast<jclass>(env->NewGlobalRef(cls));
                kCtor = env->GetMethodID(kCls, "<init>", "(IZI)V");
                assert(kCtor);
                Throwable::exceptionThrown(env);
            }

            jobject make(JNIEnv* env, MouseButton mouseButton, jboolean isPressed, jint modifiers) {
                jobject res = env->NewObject(kCls, kCtor, static_cast<int>(mouseButton), isPressed, modifiers);
                return Throwable::exceptionThrown(env) ? nullptr : res;
            }
        }

        namespace EventMouseMove {
            jclass kCls;
            jmethodID kCtor;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("io/github/humbleui/jwm/EventMouseMove");
                Throwable::exceptionThrown(env);
                kCls = static_cast<jclass>(env->NewGlobalRef(cls));
                kCtor = env->GetMethodID(kCls, "<init>", "(IIII)V");
                Throwable::exceptionThrown(env);
            }

            jobject make(JNIEnv* env, jint x, jint y, jint buttons, jint modifiers) {
                jobject res = env->NewObject(kCls, kCtor, x, y, buttons, modifiers);
                return Throwable::exceptionThrown(env) ? nullptr : res;
            }
        }


        namespace EventMouseScroll {
            jclass kCls;
            jmethodID kCtor;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("io/github/humbleui/jwm/EventMouseScroll");
                Throwable::exceptionThrown(env);
                kCls = static_cast<jclass>(env->NewGlobalRef(cls));
                kCtor = env->GetMethodID(kCls, "<init>", "(FFFFFI)V");
                Throwable::exceptionThrown(env);
            }

            jobject make(JNIEnv* env, jfloat deltaX, jfloat deltaY, jfloat deltaChars, jfloat deltaLines, jfloat deltaPages, jint modifiers) {
                jobject res = env->NewObject(kCls, kCtor, deltaX, deltaY, deltaChars, deltaLines, deltaPages, modifiers);
                return Throwable::exceptionThrown(env) ? nullptr : res;
            }
        }

        namespace EventTextInput {
            jclass kCls;
            jmethodID kCtor;

            void onLoad(JNIEnv* env) {
                // kCls = EventTextInput
                {
                    jclass cls = env->FindClass("io/github/humbleui/jwm/EventTextInput");
                    Throwable::exceptionThrown(env);
                    kCls = static_cast<jclass>(env->NewGlobalRef(cls));
                    assert(kCls);
                }

                // kCtor = EventTextInput::<init>(String text)
                {
                    kCtor = env->GetMethodID(kCls, "<init>", "(Ljava/lang/String;)V");
                    Throwable::exceptionThrown(env);
                    assert(kCtor);
                }
            }

            jobject make(JNIEnv* env, jstring text) {
                jobject res = env->NewObject(kCls, kCtor, text);
                return Throwable::exceptionThrown(env) ? nullptr : res;
            }
        }

        namespace EventTextInputMarked {
            jclass kCls;
            jmethodID kCtor;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("io/github/humbleui/jwm/EventTextInputMarked");
                Throwable::exceptionThrown(env);
                kCls = static_cast<jclass>(env->NewGlobalRef(cls));
                assert(kCls);

                kCtor = env->GetMethodID(kCls, "<init>", "(Ljava/lang/String;II)V");
                Throwable::exceptionThrown(env);
                assert(kCtor);
            }

            jobject make(JNIEnv* env, jstring text, jint selectionStart, jint selectionEnd) {
                jobject res = env->NewObject(kCls, kCtor, text, selectionStart, selectionEnd);
                return Throwable::exceptionThrown(env) ? nullptr : res;
            }
        }

        namespace EventWindowCloseRequest {
            jobject kInstance;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("io/github/humbleui/jwm/EventWindowCloseRequest");
                Throwable::exceptionThrown(env);
                jfieldID field = env->GetStaticFieldID(cls, "INSTANCE", "Lio/github/humbleui/jwm/EventWindowCloseRequest;");
                jobject instance = env->GetStaticObjectField(cls, field);
                kInstance = env->NewGlobalRef(instance);
            }
        }

        namespace EventWindowMaximize {
            jobject kInstance;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("io/github/humbleui/jwm/EventWindowMaximize");
                Throwable::exceptionThrown(env);
                jfieldID field = env->GetStaticFieldID(cls, "INSTANCE", "Lio/github/humbleui/jwm/EventWindowMaximize;");
                jobject instance = env->GetStaticObjectField(cls, field);
                kInstance = env->NewGlobalRef(instance);
            }
        }

        namespace EventWindowMinimize {
            jobject kInstance;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("io/github/humbleui/jwm/EventWindowMinimize");
                Throwable::exceptionThrown(env);
                jfieldID field = env->GetStaticFieldID(cls, "INSTANCE", "Lio/github/humbleui/jwm/EventWindowMinimize;");
                jobject instance = env->GetStaticObjectField(cls, field);
                kInstance = env->NewGlobalRef(instance);
            }
        }

        namespace EventWindowMove {
            jclass kCls;
            jmethodID kCtor;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("io/github/humbleui/jwm/EventWindowMove");
                Throwable::exceptionThrown(env);
                kCls = static_cast<jclass>(env->NewGlobalRef(cls));
                kCtor = env->GetMethodID(kCls, "<init>", "(II)V");
                Throwable::exceptionThrown(env);
            }

            jobject make(JNIEnv* env, jint left, jint top) {
                jobject res = env->NewObject(kCls, kCtor, left, top);
                return Throwable::exceptionThrown(env) ? nullptr : res;
            }
        }

        namespace EventWindowResize {
            jclass kCls;
            jmethodID kCtor;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("io/github/humbleui/jwm/EventWindowResize");
                Throwable::exceptionThrown(env);
                kCls = static_cast<jclass>(env->NewGlobalRef(cls));
                kCtor = env->GetMethodID(kCls, "<init>", "(IIII)V");
                Throwable::exceptionThrown(env);
            }

            jobject make(JNIEnv* env, jint windowWidth, jint windowHeight, jint contentWidth, jint contentHeight) {
                jobject res = env->NewObject(kCls, kCtor, windowWidth, windowHeight, contentWidth, contentHeight);
                return Throwable::exceptionThrown(env) ? nullptr : res;
            }
        }

        namespace EventWindowRestore {
            jobject kInstance;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("io/github/humbleui/jwm/EventWindowRestore");
                Throwable::exceptionThrown(env);
                jfieldID field = env->GetStaticFieldID(cls, "INSTANCE", "Lio/github/humbleui/jwm/EventWindowRestore;");
                jobject instance = env->GetStaticObjectField(cls, field);
                kInstance = env->NewGlobalRef(instance);
            }
        }
    
        namespace Screen {
            jclass kCls;
            jmethodID kCtor;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("io/github/humbleui/jwm/Screen");
                Throwable::exceptionThrown(env);
                kCls = static_cast<jclass>(env->NewGlobalRef(cls));
                kCtor = env->GetMethodID(kCls, "<init>", "(JZLio/github/humbleui/types/IRect;Lio/github/humbleui/types/IRect;F)V");
                Throwable::exceptionThrown(env);
            }

            jobject make(JNIEnv* env, jlong id, jboolean isPrimary, jwm::IRect bounds, jwm::IRect workArea, jfloat scale) {
                JNILocal<jobject> boundsObj(env, jwm::classes::IRect::toJava(env, bounds));
                JNILocal<jobject> workAreaObj(env, jwm::classes::IRect::toJava(env, workArea));
                jobject res = env->NewObject(kCls, kCtor, id, isPrimary, boundsObj.get(), workAreaObj.get(), scale);
                return Throwable::exceptionThrown(env) ? nullptr : res;
            }
        }

        namespace TextInputClient {
            jmethodID kGetRectForMarkedRange;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("io/github/humbleui/jwm/TextInputClient");
                Throwable::exceptionThrown(env);
                kGetRectForMarkedRange = env->GetMethodID(cls, "getRectForMarkedRange", "(II)Lio/github/humbleui/types/IRect;");
                Throwable::exceptionThrown(env);
            }

            jwm::IRect getRectForMarkedRange(JNIEnv* env, jobject client, jint selectionStart, jint selectionEnd) {
                JNILocal<jobject> IRect(env, env->CallObjectMethod(client, kGetRectForMarkedRange, selectionStart, selectionEnd));
                Throwable::exceptionThrown(env);
                return IRect::fromJava(env, IRect.get());
            }
        }

        namespace IRect {
            jclass kCls;
            jmethodID kCtor;
            jfieldID kLeft;
            jfieldID kTop;
            jfieldID kRight;
            jfieldID kBottom;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("io/github/humbleui/types/IRect");
                Throwable::exceptionThrown(env);
                kCls = static_cast<jclass>(env->NewGlobalRef(cls));
                kCtor = env->GetMethodID(kCls, "<init>", "(IIII)V");
                Throwable::exceptionThrown(env);
                kLeft = env->GetFieldID(cls, "_left", "I");
                Throwable::exceptionThrown(env);
                kTop = env->GetFieldID(cls, "_top", "I");
                Throwable::exceptionThrown(env);
                kRight = env->GetFieldID(cls, "_right", "I");
                Throwable::exceptionThrown(env);
                kBottom = env->GetFieldID(cls, "_bottom", "I");
                Throwable::exceptionThrown(env);
            }

            jwm::IRect fromJava(JNIEnv* env, jobject IRect) {
                int32_t left = env->GetIntField(IRect, kLeft);
                Throwable::exceptionThrown(env);
                int32_t top = env->GetIntField(IRect, kTop);
                Throwable::exceptionThrown(env);
                int32_t right = env->GetIntField(IRect, kRight);
                Throwable::exceptionThrown(env);
                int32_t bottom = env->GetIntField(IRect, kBottom);
                Throwable::exceptionThrown(env);
                return { left, top, right, bottom };
            }

            jobject toJava(JNIEnv* env, const struct IRect& rect) {
                jobject res = env->NewObject(kCls, kCtor, rect.fLeft, rect.fTop, rect.fRight, rect.fBottom);
                return Throwable::exceptionThrown(env) ? nullptr : res;
            }

            jobject toJavaXYWH(JNIEnv* env, jint left, jint top, jint width, jint height) {
                jobject res = env->NewObject(kCls, kCtor, left, top, left + width, top + height);
                return Throwable::exceptionThrown(env) ? nullptr : res;
            }
        }
    }
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_impl_Library__1nAfterLoad
  (JNIEnv* env, jclass jclass) {
    jwm::classes::Throwable::onLoad(env);
    jwm::classes::Consumer::onLoad(env);
    jwm::classes::Runnable::onLoad(env);
    jwm::classes::Native::onLoad(env);
    jwm::classes::Clipboard::onLoad(env);
    jwm::classes::ClipboardEntry::onLoad(env);
    jwm::classes::ClipboardFormat::onLoad(env);
    jwm::classes::EventWindowScreenChange::onLoad(env);
    jwm::classes::EventFrame::onLoad(env);
    jwm::classes::EventKey::onLoad(env);
    jwm::classes::EventMouseButton::onLoad(env);
    jwm::classes::EventMouseMove::onLoad(env);
    jwm::classes::EventMouseScroll::onLoad(env);
    jwm::classes::EventTextInput::onLoad(env);
    jwm::classes::EventTextInputMarked::onLoad(env);
    jwm::classes::EventWindowCloseRequest::onLoad(env);
    jwm::classes::EventWindowMaximize::onLoad(env);
    jwm::classes::EventWindowMinimize::onLoad(env);
    jwm::classes::EventWindowMove::onLoad(env);
    jwm::classes::EventWindowResize::onLoad(env);
    jwm::classes::EventWindowRestore::onLoad(env);
    jwm::classes::Screen::onLoad(env);
    jwm::classes::TextInputClient::onLoad(env);
    jwm::classes::IRect::onLoad(env);
}
