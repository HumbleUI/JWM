#include <jni.h>
#include "Context.hh"

void jwm::deleteContext(jwm::Context* instance) {
    delete instance;
}

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_Context__1nGetFinalizer
  (JNIEnv* env, jclass jclass) {
    return static_cast<jlong>(reinterpret_cast<uintptr_t>(&jwm::deleteContext));
}
