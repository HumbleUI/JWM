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

        namespace Window {
            jmethodID kOnEvent;

            void onLoad(JNIEnv* env) {
                jclass cls = env->FindClass("org/jetbrains/jwm/Window");
                Throwable::exceptionThrown(env);
                kOnEvent = env->GetMethodID(cls, "onEvent", "(Lorg/jetbrains/jwm/Event;)V");
                Throwable::exceptionThrown(env);
            }

            void onEvent(JNIEnv* env, jobject window, jobject event) {
                env->CallVoidMethod(window, kOnEvent, event);
                Throwable::exceptionThrown(env);
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

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_Library__1nAfterLoad
  (JNIEnv* env, jclass jclass) {
    jwm::classes::Throwable::onLoad(env);
    jwm::classes::Window::onLoad(env);
    jwm::classes::CloseEvent::onLoad(env);
    jwm::classes::ResizeEvent::onLoad(env);
}
