#include <jni.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include "AppX11.hh"
#include "impl/JNILocal.hh"
#include <cassert>
#include <X11/extensions/Xrandr.h>

// JNI

// ScreensX11

static bool isPrimary(RROutput primaryOutput, XRRCrtcInfo* info) {
    for (int o = 0; o < info->noutput; ++o) {
        RROutput output = info->outputs[o];
        if (output == primaryOutput) {
            return true;
        }
    }
    return false;
}

static void fillScreenFields(JNIEnv* env, jobject obj, XRRCrtcInfo* info, bool primary) {
    static jclass kScreenX11 = (jclass)env->NewGlobalRef(env->FindClass("org/jetbrains/jwm/ScreenX11"));
    static jfieldID fX = env->GetFieldID(kScreenX11, "_x", "I");
    static jfieldID fY = env->GetFieldID(kScreenX11, "_y", "I");
    static jfieldID fWidth = env->GetFieldID(kScreenX11, "_width", "I");
    static jfieldID fHeight = env->GetFieldID(kScreenX11, "_height", "I");
    static jfieldID fPrimary = env->GetFieldID(kScreenX11, "_isPrimary", "Z");

    env->SetIntField(obj, fX, info->x);
    env->SetIntField(obj, fY, info->y);
    env->SetIntField(obj, fWidth, info->width);
    env->SetIntField(obj, fHeight, info->height);
    env->SetBooleanField(obj, fPrimary, primary);
}

extern "C" JNIEXPORT jobject JNICALL Java_org_jetbrains_jwm_ScreensX11__1nX11GetPrimary(JNIEnv* env, jobject cls) noexcept {
    Display* display = jwm::app.getWindowManager().getDisplay();
    XRRScreenResources* resources = XRRGetScreenResources(display, jwm::app.getWindowManager().getRootWindow());
    RROutput primaryOutput = XRRGetOutputPrimary(display, jwm::app.getWindowManager().getRootWindow());
    static jclass kScreenX11 = (jclass)env->NewGlobalRef(env->FindClass("org/jetbrains/jwm/ScreenX11"));
    static jmethodID kCtor = env->GetMethodID(kScreenX11, "<init>", "()V");
    
    for (int i = 0; i < resources->ncrtc; ++i) {
        XRRCrtcInfo* info = XRRGetCrtcInfo(display, resources, resources->crtcs[i]);
        // skip empty monitors
        if (info->width != 0) {
            if (isPrimary(primaryOutput, info)) {
                jobject obj = env->NewObject(kScreenX11, kCtor);
                fillScreenFields(env, obj, info, true);
                XRRFreeCrtcInfo(info);
                return obj;
            }
        }

        XRRFreeCrtcInfo(info);
    }

    // fill with first non-empty

    for (int i = 0; i < resources->ncrtc; ++i) {
        XRRCrtcInfo* info = XRRGetCrtcInfo(display, resources, resources->crtcs[i]);
        // skip empty monitors
        if (info->width != 0) {
            jobject obj = env->NewObject(kScreenX11, kCtor);
            fillScreenFields(env, obj, info, true);
            XRRFreeCrtcInfo(info);
            return obj;
        }

        XRRFreeCrtcInfo(info);
    }


    return nullptr;
}

extern "C" JNIEXPORT jobjectArray JNICALL Java_org_jetbrains_jwm_ScreensX11__1nX11GetAll(JNIEnv* env, jobject cls) noexcept {
    Display* display = jwm::app.getWindowManager().getDisplay();
    XRRScreenResources* resources = XRRGetScreenResources(display, jwm::app.getWindowManager().getRootWindow());
    RROutput primaryOutput = XRRGetOutputPrimary(display, jwm::app.getWindowManager().getRootWindow());
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

    for (int i = 0; i < count; ++i) {
        XRRCrtcInfo* info = XRRGetCrtcInfo(display, resources, resources->crtcs[i]);
        // skip empty monitors
        if (info->width != 0) {
            jwm::JNILocal<jobject> obj(env, env->NewObject(kScreenX11, kCtor));
            fillScreenFields(env, obj.get(), info, isPrimary(primaryOutput, info));
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