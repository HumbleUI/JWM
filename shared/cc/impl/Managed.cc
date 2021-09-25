#include <jni.h>
#include <cstdint>

typedef void (*FreeFunction)(void*);

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_impl_Managed__1nInvokeFinalizer
  (JNIEnv* env, jclass jclass, jlong finalizerPtr, jlong ptr) {
    void* instance = reinterpret_cast<void*>(static_cast<uintptr_t>(ptr));
    FreeFunction finalizer = reinterpret_cast<FreeFunction>(static_cast<uintptr_t>(finalizerPtr));
    finalizer(instance);
}