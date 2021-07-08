#include <jni.h>

// JNI

extern "C" JNIEXPORT jobject JNICALL Java_org_jetbrains_jwm_Screens__1nInstantiate(JNIEnv* env, jclass cls) {
    jclass kCls = env->FindClass("org/jetbrains/jwm/ScreensX11");
    jmethodID kCtor = env->GetMethodID(kCls, "<init>", "()V");
    jobject res = env->NewObject(kCls, kCtor);
    return res;
}
