#include <jni.h>
#include "AppX11.hh"
#include <X11/Xresource.h>
#include <cstdlib>

jwm::AppX11 jwm::app;


float jwm::AppX11::getScale() {
    char *resourceString = XResourceManagerString(wm.display);
    XrmDatabase db;
    XrmValue value;
    char *type = NULL;

    static struct once {
        once() {
            XrmInitialize();
        }
    } once;

    db = XrmGetStringDatabase(resourceString);

    if (resourceString) {
        if (XrmGetResource(db, "Xft.dpi", "String", &type, &value)) {
            if (value.addr) {
                return atof(value.addr) / 96.f;
            }
        }
    }

    return 1.f;
}

extern "C" JNIEXPORT jobject JNICALL Java_org_jetbrains_jwm_App__1nMakeScreens(JNIEnv* env, jclass cls) {
    jclass kCls = env->FindClass("org/jetbrains/jwm/ScreensX11");
    jmethodID kCtor = env->GetMethodID(kCls, "<init>", "()V");
    jobject res = env->NewObject(kCls, kCtor);
    return res;
}

void jwm::AppX11::init(JNIEnv* jniEnv) {
    _jniEnv = jniEnv;
}

void jwm::AppX11::start() {
    wm.runLoop();
}

void jwm::AppX11::terminate() {
    wm.terminate();
}

// JNI

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_App__1nInit(JNIEnv* env, jclass jclass) {
    jwm::app.init(env);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_App_start(JNIEnv* env, jclass jclass) {
    jwm::app.start();
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_App_terminate(JNIEnv* env, jclass jclass) {
    jwm::app.terminate();
}

extern"C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) {
    return JNI_VERSION_1_2;
}