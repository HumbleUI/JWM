#include <AppWin32.hh>
#include <iostream>

// Globally accessible instance
// Will be created as soon as java runtime load this library

jwm::AppWin32 jwm::AppWin32::gInstance;

void jwm::AppWin32::init(JNIEnv *jniEnv) {
    _jniEnv = jniEnv;

    if (!_windowManager.init()) {
        jclass Exception = _jniEnv->FindClass("java/lang/Exception");
        _jniEnv->ThrowNew(Exception, "Failed to initialize Win32 Window Manager");
    }
}

int jwm::AppWin32::start() {
    return _windowManager.runMainLoop();
}

void jwm::AppWin32::terminate() {
    _windowManager.requestTerminate();
}

void jwm::AppWin32::sendError(const char *what) {
    std::cerr << "jwm::Error: " << what << std::endl;
}

// JNI

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_App__1nInit
        (JNIEnv* env, jclass jclass) {
    jwm::AppWin32::getInstance().init(env);
}

extern "C" JNIEXPORT jint JNICALL Java_org_jetbrains_jwm_App_start
        (JNIEnv* env, jclass jclass) {
    return jwm::AppWin32::getInstance().start();
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_App_terminate
        (JNIEnv* env, jclass jclass) {
    jwm::AppWin32::getInstance().terminate();
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_App_runOnUIThread
        (JNIEnv* env, jclass jclass, jobject callback) {
    // todo, maybe create queue in the app manager for such kind of runnable objects
}