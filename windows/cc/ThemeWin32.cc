#include <jni.h>
#include <AppWin32.hh>
#include <VersionHelpers.h>
#include <WinUser.h>
#include <Log.hh>

using namespace jwm;

// JNI

extern "C" JNIEXPORT bool JNICALL Java_io_github_humbleui_jwm_Theme__1nIsHighContrast
  (JNIEnv* env, jclass jclass) {
    HIGHCONTRASTA highContrast;
    highContrast.cbSize = sizeof(HIGHCONTRASTA);
    highContrast.dwFlags = 0;
    highContrast.lpszDefaultScheme = nullptr;
    bool isOk = SystemParametersInfoA(SPI_GETHIGHCONTRAST, 0, &highContrast, 0);
    if (!isOk) {
        JWM_LOG("Failed to get SystemParametersInfoA for high contrast");
        return false;
    }
    bool result = (HCF_HIGHCONTRASTON & highContrast.dwFlags) == 1;
    JWM_VERBOSE("is HighContrast? '" << result << "'");
    return result;
}


extern "C" JNIEXPORT bool JNICALL Java_io_github_humbleui_jwm_Theme__1nIsDark
  (JNIEnv* env, jclass jclass) {
    return false;
}
