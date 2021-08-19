#include <iostream>
#include <jni.h>
#include "Window.hh"
#include "impl/Library.hh"

jwm::Window::~Window() {
    if (fEventListener)
        fEnv->DeleteGlobalRef(fEventListener);
}

void jwm::Window::dispatch(jobject event) {
  if (fEventListener)
      jwm::classes::Consumer::accept(fEnv, fEventListener, event);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_Window__1nSetEventListener
  (JNIEnv* env, jobject obj, jobject listener) {
    jwm::Window* instance = reinterpret_cast<jwm::Window*>(jwm::classes::Native::fromJava(env, obj));
    if (instance->fEventListener)
        env->DeleteGlobalRef(instance->fEventListener);
    instance->fEventListener = listener ? env->NewGlobalRef(listener) : nullptr;
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_Window__1nSetTextInputClient
  (JNIEnv* env, jobject obj, jobject client) {
    jwm::Window* instance = reinterpret_cast<jwm::Window*>(jwm::classes::Native::fromJava(env, obj));
    if (instance->fTextInputClient)
        env->DeleteGlobalRef(instance->fTextInputClient);
    instance->fTextInputClient = client ? env->NewGlobalRef(client) : nullptr;
}