#include <AppWin32.hh>
#include <impl/Library.hh>
#include <Log.hh>
#include <iostream>

// Globally accessible instance
// Will be created as soon as java runtime load this library
jwm::AppWin32 jwm::AppWin32::gInstance;

void jwm::AppWin32::init(JNIEnv *jniEnv) {
    _jniEnv = jniEnv;

    if (!_windowManager.init()) {
        JWM_LOG("Failed to initialize Win32 Window Manager");
        classes::Throwable::throwRuntimeException(_jniEnv, "Failed to initialize Win32 Window Manager");
    }
}

int jwm::AppWin32::start() {
    int result = 0;

    while (!isTerminateRequested()) {
        result = _windowManager.start();
    }

    return result;
}

void jwm::AppWin32::terminate() {
    _terminateRequested = true;
    _windowManager.postMessage(WM_CLOSE, nullptr);
}

bool jwm::AppWin32::isTerminateRequested() const {
    return _terminateRequested;
}

void jwm::AppWin32::enqueueCallback(jobject callback) {
    _windowManager.postMessage(JWM_WM_UI_CALLBACK_EVENT, callback);
}

const std::vector<jwm::ScreenWin32> &jwm::AppWin32::getScreens() {
    _screens.clear();
    EnumDisplayMonitors(nullptr, nullptr, (MONITORENUMPROC) enumMonitorFunc, 0);
    return _screens;
}

BOOL jwm::AppWin32::enumMonitorFunc(HMONITOR monitor, HDC dc, LPRECT rect, LPARAM data) {
    ScreenWin32 screen = ScreenWin32::fromHMonitor(monitor);
    AppWin32::getInstance()._screens.push_back(screen);
    return TRUE;
}

LRESULT jwm::AppWin32::processEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == JWM_WM_UI_CALLBACK_EVENT) {
        JNIEnv* env = getJniEnv();
        jobject callbackRef = reinterpret_cast<jobject>(lParam);
        classes::Runnable::run(env, callbackRef);
        env->DeleteGlobalRef(callbackRef);
        return true;
    }

    return false;
}

// JNI

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_App__1nInit
        (JNIEnv* env, jclass jclass) {
    jwm::AppWin32::getInstance().init(env);
}

extern "C" JNIEXPORT jint JNICALL Java_io_github_humbleui_jwm_App__1nStart
        (JNIEnv* env, jclass jclass) {
    return jwm::AppWin32::getInstance().start();
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_App__1nTerminate
        (JNIEnv* env, jclass jclass) {
    jwm::AppWin32::getInstance().terminate();
}

extern "C" JNIEXPORT jobjectArray JNICALL Java_io_github_humbleui_jwm_App__1nGetScreens
        (JNIEnv* env, jclass jclass) {
    auto& app = jwm::AppWin32::getInstance();
    auto& screens = app.getScreens();
    auto screensCount = static_cast<jsize>(screens.size());

    jobjectArray array = env->NewObjectArray(screensCount, jwm::classes::Screen::kCls, nullptr);

    if (jwm::classes::Throwable::exceptionThrown(env))
        return nullptr;

    for (jsize i = 0; i < screensCount; i++) {
        const jwm::ScreenWin32& screenData = screens[i];
        jwm::JNILocal<jobject> screen(env, screenData.toJni(env));
        env->SetObjectArrayElement(array, i, screen.get());
    }

    return array;
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_App__1nRunOnUIThread
        (JNIEnv* env, jclass cls, jobject callback) {
    jwm::AppWin32& app = jwm::AppWin32::getInstance();
    jobject callbackRef = env->NewGlobalRef(callback);
    app.enqueueCallback(callbackRef);
}
