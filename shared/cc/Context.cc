#include <jni.h>
#include "Context.hh"
#include "Window.hh"
#include "impl/Library.hh"

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_Context_swapBuffers
  (JNIEnv* env, jobject obj) {
    jwm::Context* instance = reinterpret_cast<jwm::Context*>(jwm::classes::Native::fromJava(env, obj));
    instance->swapBuffers();
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_Context_resize
  (JNIEnv* env, jobject obj) {
    jwm::Context* instance = reinterpret_cast<jwm::Context*>(jwm::classes::Native::fromJava(env, obj));
    instance->resize();
}

extern "C" JNIEXPORT jint JNICALL Java_org_jetbrains_jwm_Context_getWidth
  (JNIEnv* env, jobject obj) {
    jwm::Context* instance = reinterpret_cast<jwm::Context*>(jwm::classes::Native::fromJava(env, obj));
    return instance->fWidth;
}

extern "C" JNIEXPORT jint JNICALL Java_org_jetbrains_jwm_Context_getHeight
  (JNIEnv* env, jobject obj) {
    jwm::Context* instance = reinterpret_cast<jwm::Context*>(jwm::classes::Native::fromJava(env, obj));
    return instance->fHeight;
}

extern "C" JNIEXPORT jfloat JNICALL Java_org_jetbrains_jwm_Context_getScale
  (JNIEnv* env, jobject obj) {
    jwm::Context* instance = reinterpret_cast<jwm::Context*>(jwm::classes::Native::fromJava(env, obj));
    return instance->fScale;
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_Context__1nAttach
  (JNIEnv* env, jobject obj, jobject windowObj) {
    jwm::Context* instance = reinterpret_cast<jwm::Context*>(jwm::classes::Native::fromJava(env, obj));
    jwm::Window* window = reinterpret_cast<jwm::Window*>(jwm::classes::Native::fromJava(env, windowObj));
    instance->fWindow = window;
    instance->attach(window);
    instance->invalidate();
    instance->resize();
    window->fContext = jwm::ref(instance);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_Context__1nDetach
  (JNIEnv* env, jobject obj) {
    jwm::Context* instance = reinterpret_cast<jwm::Context*>(jwm::classes::Native::fromJava(env, obj));
    jwm::Window* window = instance->fWindow;
    jwm::unref(&window->fContext);
    instance->detach();
    instance->fWindow = nullptr;
}
