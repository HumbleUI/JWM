#include <jni.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include "AppX11.hh"
#include "impl/JNILocal.hh"
#include <cassert>
#include <X11/extensions/Xrandr.h>

// JNI

// ScreensX11
extern "C" JNIEXPORT jobject JNICALL Java_org_jetbrains_jwm_Screens__1nInstantiate(JNIEnv* env, jclass cls) {
    jclass kCls = env->FindClass("org/jetbrains/jwm/ScreensX11");
    jmethodID kCtor = env->GetMethodID(kCls, "<init>", "()V");
    jobject res = env->NewObject(kCls, kCtor);
    return res;
}

extern "C" JNIEXPORT jobjectArray JNICALL Java_org_jetbrains_jwm_ScreensX11__1nX11GetAll(JNIEnv* env, jclass cls) noexcept {
    Display* display = jwm::app.getWindowManager().getDisplay();
    XRRScreenResources* resources = XRRGetScreenResources(display, jwm::app.getWindowManager().getRootWindow());
    int count = resources->ncrtc;


    // skip empty monitors
    for (int i = 0; i < resources->ncrtc; ++i) {
        XRRCrtcInfo* info = XRRGetCrtcInfo(display, resources, resources->crtcs[i]);
        if (info->width == 0) {
            count -= 1;
        }
        XRRFreeCrtcInfo(info);
    }

    jobjectArray array = env->NewObjectArray(count, env->FindClass("org/jetbrains/jwm/IScreen"), 0);

    static jclass kScreenX11 = (jclass)env->NewGlobalRef(env->FindClass("org/jetbrains/jwm/ScreenX11"));
    static jmethodID kCtor = env->GetMethodID(kScreenX11, "<init>", "()V");
    static jfieldID fWidth = env->GetFieldID(kScreenX11, "_width", "I");
    static jfieldID fHeight = env->GetFieldID(kScreenX11, "_height", "I");

    

    for (int i = 0; i < count; ++i) {
        XRRCrtcInfo* info = XRRGetCrtcInfo(display, resources, resources->crtcs[i]);
        // skip empty monitors
        if (info->width != 0) {
            jwm::JNILocal<jobject> obj(env, env->NewObject(kScreenX11, kCtor));
            env->SetIntField(obj.get(), fWidth, info->width);
            env->SetIntField(obj.get(), fHeight, info->height);
            env->SetObjectArrayElement(array, i, obj.get());
        }
        XRRFreeCrtcInfo(info);
    }
    XRRFreeScreenResources(resources);

    return array;
}


// ScreenX11

extern "C" JNIEXPORT jfloat JNICALL Java_org_jetbrains_jwm_ScreenX11_getScale(JNIEnv* env, jobject screenX11) {
    return jwm::app.getScale();
}