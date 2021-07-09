#include <jni.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include "AppX11.hh"

// JNI

extern "C" JNIEXPORT jobject JNICALL Java_org_jetbrains_jwm_Screens__1nInstantiate(JNIEnv* env, jclass cls) {
    jclass kCls = env->FindClass("org/jetbrains/jwm/ScreensX11");
    jmethodID kCtor = env->GetMethodID(kCls, "<init>", "()V");
    jobject res = env->NewObject(kCls, kCtor);
    return res;
}


extern "C" JNIEXPORT jint JNICALL Java_org_jetbrains_jwm_ScreensX11__1nX11GetScreenCount(JNIEnv* env, jclass cls) {
    
    return XScreenCount(jwm::app.getWindowManager().getDisplay());
}
