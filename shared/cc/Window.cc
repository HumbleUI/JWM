#include <iostream>
#include <jni.h>
#include "Window.hh"
#include "impl/JNILocal.hh"
#include "impl/Library.hh"

jwm::Window::~Window() {
    fEnv->DeleteGlobalRef(fWindow);
}

void jwm::Window::dispatch(jobject event) {
    jwm::classes::Consumer::accept(fEnv, fWindow, event);
}

bool jwm::Window::getRectForMarkedRange(jint selectionStart, jint selectionEnd, jwm::IRect& rect) const {
    JNILocal<jobject> client(fEnv, fEnv->GetObjectField(fWindow, jwm::classes::Window::kTextInputClient));
    jwm::classes::Throwable::exceptionThrown(fEnv);
    if (client.get()) {
        rect = jwm::classes::TextInputClient::getRectForMarkedRange(fEnv, client.get(), selectionStart, selectionEnd);
        return true;
    } else
        return false;
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_Window__1nInit
  (JNIEnv* env, jobject obj) {
    jwm::Window* instance = reinterpret_cast<jwm::Window*>(jwm::classes::Native::fromJava(env, obj));
    instance->fWindow = env->NewGlobalRef(obj);
}
