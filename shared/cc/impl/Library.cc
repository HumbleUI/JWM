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
            jmethodID kPrintStackTrace;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("java/lang/Throwable");
                kPrintStackTrace = env->GetMethodID(cls, "printStackTrace", "()V");
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
                jclass cls = env->FindClass("org/jetbrains/jwm/impl/Native");
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
                jclass cls = env->FindClass("org/jetbrains/jwm/Clipboard");
                Throwable::exceptionThrown(env);
                kCls = static_cast<jclass>(env->NewGlobalRef(cls));
                assert(kCls);

                // public static ClipboardFormat _registerPredefinedFormat(String formatId)
                kRegisterPredefinedFormat = env->GetStaticMethodID(kCls,
                "_registerPredefinedFormat",
                "(Ljava/lang/String;)Lorg/jetbrains/jwm/ClipboardFormat;"
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
                jclass cls = env->FindClass("org/jetbrains/jwm/ClipboardEntry");
                Throwable::exceptionThrown(env);
                kCls = static_cast<jclass>(env->NewGlobalRef(cls));
                assert(kCls);

                // public static ClipboardEntry make(ClipboardFormat format, byte[] data)
                kMake = env->GetStaticMethodID(kCls, "make",
                "(Lorg/jetbrains/jwm/ClipboardFormat;[B)"
                    "Lorg/jetbrains/jwm/ClipboardEntry;"
                );
                Throwable::exceptionThrown(env);
                assert(kMake);

                kFormat = env->GetFieldID(kCls, "_format", "Lorg/jetbrains/jwm/ClipboardFormat;");
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
                jclass cls = env->FindClass("org/jetbrains/jwm/ClipboardFormat");
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
                jclass cls = env->FindClass("org/jetbrains/jwm/EventWindowScreenChange");
                Throwable::exceptionThrown(env);
                jfieldID field = env->GetStaticFieldID(cls, "INSTANCE", "Lorg/jetbrains/jwm/EventWindowScreenChange;");
                jobject instance = env->GetStaticObjectField(cls, field);
                kInstance = env->NewGlobalRef(instance);
            }
        }

        namespace EventFrame {
            jobject kInstance;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("org/jetbrains/jwm/EventFrame");
                Throwable::exceptionThrown(env);
                jfieldID field = env->GetStaticFieldID(cls, "INSTANCE", "Lorg/jetbrains/jwm/EventFrame;");
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
                    jclass cls = env->FindClass("org/jetbrains/jwm/EventKey");
                    Throwable::exceptionThrown(env);
                    kCls = static_cast<jclass>(env->NewGlobalRef(cls));
                    assert(kCls);
                }

                // kCtor = EventKey::<init>(Key key, boolean isPressed, int modifiers)
                {
                    kCtor = env->GetMethodID(kCls, "<init>", "(IZI)V");
                    Throwable::exceptionThrown(env);
                    assert(kCtor);
                }
            }

            jobject make(JNIEnv* env, Key keyCode, jboolean isPressed, jint modifiers) {
                jobject res = env->NewObject(kCls, kCtor, static_cast<int>(keyCode), isPressed, modifiers);
                return Throwable::exceptionThrown(env) ? nullptr : res;
            }
        }

        namespace EventMouseButton {
            jclass kCls;
            jmethodID kCtor;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("org/jetbrains/jwm/EventMouseButton");
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
                jclass cls = env->FindClass("org/jetbrains/jwm/EventMouseMove");
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
                jclass cls = env->FindClass("org/jetbrains/jwm/EventMouseScroll");
                Throwable::exceptionThrown(env);
                kCls = static_cast<jclass>(env->NewGlobalRef(cls));
                kCtor = env->GetMethodID(kCls, "<init>", "(FFI)V");
                Throwable::exceptionThrown(env);
            }

            jobject make(JNIEnv* env, jfloat dx, jfloat dy, jint modifiers) {
                jobject res = env->NewObject(kCls, kCtor, dx, dy, modifiers);
                return Throwable::exceptionThrown(env) ? nullptr : res;
            }
        }

        namespace EventTextInput {
            jclass kCls;
            jmethodID kCtor;

            void onLoad(JNIEnv* env) {
                // kCls = EventTextInput
                {
                    jclass cls = env->FindClass("org/jetbrains/jwm/EventTextInput");
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
                jclass cls = env->FindClass("org/jetbrains/jwm/EventTextInputMarked");
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
                jclass cls = env->FindClass("org/jetbrains/jwm/EventWindowCloseRequest");
                Throwable::exceptionThrown(env);
                jfieldID field = env->GetStaticFieldID(cls, "INSTANCE", "Lorg/jetbrains/jwm/EventWindowCloseRequest;");
                jobject instance = env->GetStaticObjectField(cls, field);
                kInstance = env->NewGlobalRef(instance);
            }
        }

        namespace EventWindowMove {
            jclass kCls;
            jmethodID kCtor;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("org/jetbrains/jwm/EventWindowMove");
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
                jclass cls = env->FindClass("org/jetbrains/jwm/EventWindowResize");
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
    
        namespace Screen {
            jclass kCls;
            jmethodID kCtor;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("org/jetbrains/jwm/Screen");
                Throwable::exceptionThrown(env);
                kCls = static_cast<jclass>(env->NewGlobalRef(cls));
                kCtor = env->GetMethodID(kCls, "<init>", "(JIIIIFZ)V");
                Throwable::exceptionThrown(env);
            }

            jobject make(JNIEnv* env, jlong id, jint x, jint y, jint width, jint height, jfloat scale, jboolean isPrimary) {
                jobject res = env->NewObject(kCls, kCtor, id, x, y, width, height, scale, isPrimary);
                return Throwable::exceptionThrown(env) ? nullptr : res;
            }
        }

        namespace TextInputClient {
            jmethodID kGetRectForMarkedRange;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("org/jetbrains/jwm/TextInputClient");
                Throwable::exceptionThrown(env);
                kGetRectForMarkedRange = env->GetMethodID(cls, "getRectForMarkedRange", "(II)Lorg/jetbrains/jwm/UIRect;");
                Throwable::exceptionThrown(env);
            }

            jwm::UIRect getRectForMarkedRange(JNIEnv* env, jobject client, jint selectionStart, jint selectionEnd) {
                JNILocal<jobject> uiRect(env, env->CallObjectMethod(client, kGetRectForMarkedRange, selectionStart, selectionEnd));
                Throwable::exceptionThrown(env);
                return UIRect::fromJava(env, uiRect.get());
            }
        }

        namespace UIRect {
            jclass kCls;
            jmethodID kCtor;
            jfieldID kLeft;
            jfieldID kTop;
            jfieldID kRight;
            jfieldID kBottom;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("org/jetbrains/jwm/UIRect");
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

            jwm::UIRect fromJava(JNIEnv* env, jobject uirect) {
                int32_t left = env->GetIntField(uirect, kLeft);
                Throwable::exceptionThrown(env);
                int32_t top = env->GetIntField(uirect, kTop);
                Throwable::exceptionThrown(env);
                int32_t right = env->GetIntField(uirect, kRight);
                Throwable::exceptionThrown(env);
                int32_t bottom = env->GetIntField(uirect, kBottom);
                Throwable::exceptionThrown(env);
                return { left, top, right, bottom };
            }

            jobject toJava(JNIEnv* env, const struct UIRect& rect) {
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

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_impl_Library__1nAfterLoad
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
    jwm::classes::EventWindowMove::onLoad(env);
    jwm::classes::EventWindowResize::onLoad(env);
    jwm::classes::Screen::onLoad(env);
    jwm::classes::TextInputClient::onLoad(env);
    jwm::classes::UIRect::onLoad(env);
}
