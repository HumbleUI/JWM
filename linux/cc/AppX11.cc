#include <jni.h>
#include "AppX11.hh"
#include <X11/Xresource.h>
#include <cstdlib>
#include <impl/Library.hh>
#include <impl/JNILocal.hh>
#include <X11/extensions/sync.h>
#include <X11/extensions/Xrandr.h>

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


extern "C" JNIEXPORT jobjectArray JNICALL Java_org_jetbrains_jwm_App_getScreens(JNIEnv* env, jobject cls) noexcept {
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


    jobjectArray array = env->NewObjectArray(count, jwm::classes::Screen::kCls, 0);    

    float dpi = jwm::app.getScale();

    for (int i = 0; i < count; ++i) {
        XRRCrtcInfo* info = XRRGetCrtcInfo(display, resources, resources->crtcs[i]);
        // skip empty monitors
        if (info->width != 0) {
            bool isPrimary = false;
            for (int o = 0; o < info->noutput; ++o) {
                RROutput output = info->outputs[o];
                if (output == primaryOutput) {
                    isPrimary = true;
                    break;
                }
            }

            jwm::JNILocal<jobject> obj(env, jwm::classes::Screen::make(
                env,
                info->outputs[0],
                info->x,
                info->y,
                info->width,
                info->height,
                dpi,
                isPrimary
            ));
            env->SetObjectArrayElement(array, i, obj.get());
        }

        XRRFreeCrtcInfo(info);
    }
    XRRFreeScreenResources(resources);

    return array;
}
