#include <jni.h>
#include "App.hh"

jwm::App jwm::app;


void jwm::App::init(JNIEnv* jniEnv) {
    _jniEnv = jniEnv;
}

void jwm::App::start() {
    wm.runLoop();
}

void jwm::App::terminate() {
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