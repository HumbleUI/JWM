#include <jni.h>
#include "Library.hh"
#include "RefCounted.hh"

void jwm::unrefRefCounted(jwm::RefCounted* instance) {
    instance->unref();
}

extern "C" JNIEXPORT jlong JNICALL Java_io_github_humbleui_jwm_impl_RefCounted__1nGetFinalizer
  (JNIEnv* env, jclass jclass) {
    return static_cast<jlong>(reinterpret_cast<uintptr_t>(&jwm::unrefRefCounted));
}

extern "C" JNIEXPORT jint JNICALL Java_io_github_humbleui_jwm_impl_RefCounted_getRefCount
  (JNIEnv* env, jobject obj) {
    jwm::RefCounted* instance = reinterpret_cast<jwm::RefCounted*>(jwm::classes::Native::fromJava(env, obj));
    return instance->getRefCount();
}
