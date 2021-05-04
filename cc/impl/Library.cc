#include <iostream>
#include <jni.h>
#include "Library.hh"

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
                    jwm::AutoLocal<jthrowable> th(env, env->ExceptionOccurred());
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

            void accept(JNIEnv* env, jobject consumer, jobject event) {
                env->CallVoidMethod(consumer, kAccept, event);
                Throwable::exceptionThrown(env);
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

        namespace CloseEvent {
            jclass kCls;
            jmethodID kCtor;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("org/jetbrains/jwm/CloseEvent");
                Throwable::exceptionThrown(env);
                kCls = static_cast<jclass>(env->NewGlobalRef(cls));
                kCtor = env->GetMethodID(kCls, "<init>", "()V");
                Throwable::exceptionThrown(env);
            }

            jobject make(JNIEnv* env) {
                jobject res = env->NewObject(kCls, kCtor);
                return Throwable::exceptionThrown(env) ? nullptr : res;
            }
        }

        namespace MouseMoveEvent {
            jclass kCls;
            jmethodID kCtor;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("org/jetbrains/jwm/MouseMoveEvent");
                Throwable::exceptionThrown(env);
                kCls = static_cast<jclass>(env->NewGlobalRef(cls));
                kCtor = env->GetMethodID(kCls, "<init>", "(II)V");
                Throwable::exceptionThrown(env);
            }

            jobject make(JNIEnv* env, jint x, jint y) {
                jobject res = env->NewObject(kCls, kCtor, x, y);
                return Throwable::exceptionThrown(env) ? nullptr : res;
            }
        }

        namespace PaintEvent {
            jclass kCls;
            jmethodID kCtor;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("org/jetbrains/jwm/PaintEvent");
                Throwable::exceptionThrown(env);
                kCls = static_cast<jclass>(env->NewGlobalRef(cls));
                kCtor = env->GetMethodID(kCls, "<init>", "()V");
                Throwable::exceptionThrown(env);
            }

            jobject make(JNIEnv* env) {
                jobject res = env->NewObject(kCls, kCtor);
                return Throwable::exceptionThrown(env) ? nullptr : res;
            }
        }

        namespace ResizeEvent {
            jclass kCls;
            jmethodID kCtor;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("org/jetbrains/jwm/ResizeEvent");
                Throwable::exceptionThrown(env);
                kCls = static_cast<jclass>(env->NewGlobalRef(cls));
                kCtor = env->GetMethodID(kCls, "<init>", "(II)V");
                Throwable::exceptionThrown(env);
            }

            jobject make(JNIEnv* env, jint width, jint height) {
                jobject res = env->NewObject(kCls, kCtor, width, height);
                return Throwable::exceptionThrown(env) ? nullptr : res;
            }
        }
    }
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_impl_Library__1nAfterLoad
  (JNIEnv* env, jclass jclass) {
    jwm::classes::Throwable::onLoad(env);
    jwm::classes::Consumer::onLoad(env);
    jwm::classes::Native::onLoad(env);
    jwm::classes::CloseEvent::onLoad(env);
    jwm::classes::MouseMoveEvent::onLoad(env);
    jwm::classes::PaintEvent::onLoad(env);
    jwm::classes::ResizeEvent::onLoad(env);
}
