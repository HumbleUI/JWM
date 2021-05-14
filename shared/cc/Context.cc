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
