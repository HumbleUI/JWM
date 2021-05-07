#include <jni.h>
#include "Context.hh"
#include "Window.hh"
#include "impl/Library.hh"

void jwm::deleteWindow(jwm::Window* instance) {
    delete instance;
}

void jwm::Window::onEvent(jobject event) {
  if (fEventListener)
      jwm::classes::Consumer::accept(fEnv, fEventListener, event);
}

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_Window__1nGetFinalizer
  (JNIEnv* env, jclass jclass) {
    return static_cast<jlong>(reinterpret_cast<uintptr_t>(&jwm::deleteWindow));
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_Window__1nSetEventListener
  (JNIEnv* env, jobject obj, jobject listener) {
    jwm::Window* instance = reinterpret_cast<jwm::Window*>(jwm::classes::Native::fromJava(env, obj));
    if (instance->fEventListener)
        env->DeleteGlobalRef(instance->fEventListener);
    instance->fEventListener = listener ? env->NewGlobalRef(listener) : nullptr;
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_Window__1nAttach
  (JNIEnv* env, jobject obj, jobject contextObj) {
    jwm::Window* instance = reinterpret_cast<jwm::Window*>(jwm::classes::Native::fromJava(env, obj));
    jwm::Context* context = reinterpret_cast<jwm::Context*>(jwm::classes::Native::fromJava(env, contextObj));
    context->attach(instance);
    instance->fContext = context;
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_Window__1nDetach
  (JNIEnv* env, jobject obj) {
    jwm::Window* instance = reinterpret_cast<jwm::Window*>(jwm::classes::Native::fromJava(env, obj));
    instance->fContext = nullptr;
}
