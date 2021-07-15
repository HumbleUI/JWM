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

const std::vector<jwm::Screen> &jwm::AppWin32::getScreens() {
    _screens.clear();
    EnumDisplayMonitors(NULL, NULL, (MONITORENUMPROC) enumMonitorFunc, 0);
    return _screens;
}

BOOL jwm::AppWin32::enumMonitorFunc(HMONITOR monitor, HDC dc, LPRECT rect, LPARAM data) {
    Screen screen{};
    screen.hMonitor = monitor;

    MONITORINFO monitorinfo;
    ZeroMemory(&monitorinfo, sizeof(monitorinfo));
    monitorinfo.cbSize = sizeof(monitorinfo);

    GetMonitorInfoW(monitor, &monitorinfo);

    auto& area = monitorinfo.rcMonitor;

    // Position
    {
        screen.x = area.left;
        screen.y = area.top;
    }

    // Size
    {
        screen.width = area.right - area.left;
        screen.height = area.bottom - area.top;
    }

    // Scale
    {
        DEVICE_SCALE_FACTOR scaleFactor;
        GetScaleFactorForMonitor(monitor, &scaleFactor);

        if (scaleFactor == DEVICE_SCALE_FACTOR_INVALID)
            scaleFactor = JWM_DEFAULT_DEVICE_SCALE;

        screen.scale = (float) scaleFactor / (float) SCALE_100_PERCENT;
    }

    // Is primary
    {
        screen.isPrimary = monitorinfo.dwFlags & MONITORINFOF_PRIMARY;
    }

    AppWin32::getInstance()._screens.push_back(screen);

    return TRUE;
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

extern "C" JNIEXPORT jobjectArray JNICALL Java_org_jetbrains_jwm_App_getScreens
        (JNIEnv* env, jclass jclass) {
    jobjectArray array = env->NewObjectArray(1, jwm::classes::Screen::kCls, 0);

    if (jwm::classes::Throwable::exceptionThrown(env))
        return nullptr;

    auto& app = jwm::AppWin32::getInstance();
    auto& screens = app.getScreens();

    for (size_t i = 0; i < screens.size(); i++) {
        const jwm::Screen& screenData = screens[i];
        jlong id = reinterpret_cast<jlong>(screenData.hMonitor);

        jobject screen = jwm::classes::Screen::make(
            env,
            id,
            screenData.x, screenData.y,
            screenData.width, screenData.height,
            screenData.scale,
            screenData.isPrimary
        );

        env->SetObjectArrayElement(array, i, screen);
    }

    return array;
}