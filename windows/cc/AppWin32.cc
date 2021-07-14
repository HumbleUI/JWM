#include <AppWin32.hh>
#include <impl/Library.hh>
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
    auto env = getJniEnv();

    while (!_terminateRequested.load()) {
        int result = _windowManager.iteration();

        if (result)
            return result;

        // Process UI thread callbacks
        std::vector<jobject> process;
        std::swap(process, _uiThreadCallbacks);

        for (auto callback: process) {
            jwm::classes::Runnable::run(env, callback);
            env->DeleteGlobalRef(callback);
        }
    }

    // Release enqueued but not executed callbacks
    for (auto callback: _uiThreadCallbacks)
        env->DeleteGlobalRef(callback);

    return 0;
}

void jwm::AppWin32::terminate() {
    _terminateRequested.store(true);
}

void jwm::AppWin32::sendError(const char *what) {
    std::cerr << "jwm::Error: " << what << std::endl;
}

void jwm::AppWin32::enqueueUIThreadCallback(jobject callback) {
    _uiThreadCallbacks.push_back(callback);
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
    auto callbackRef = env->NewGlobalRef(callback);
    jwm::AppWin32::getInstance().enqueueUIThreadCallback(callbackRef);
}