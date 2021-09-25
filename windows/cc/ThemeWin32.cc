#include "ThemeWin32.hh"

#include <Uxtheme.h>
#include <impl/Library.hh>
#include <AppWin32.hh>
#include <VersionHelpers.h>
#include <WinUser.h>
#include <stdio.h>
#include <Log.hh>
#include <PlatformWin32.hh>
#include "Theme.hh"


bool jwm::ThemeWin32::isHighContrast() {
    HIGHCONTRASTA highContrast;
    highContrast.cbSize = sizeof(HIGHCONTRASTA);
    highContrast.dwFlags = 0;
    highContrast.lpszDefaultScheme = nullptr;
    bool isOk = SystemParametersInfoA(SPI_GETHIGHCONTRAST, 0, &highContrast, 0);
    if (!isOk) {
        JWM_VERBOSE("Failed to get SystemParametersInfoA for high contrast");
        return false;
    }
    JWM_VERBOSE("is HighContrast? '"
                << ((HCF_HIGHCONTRASTON & highContrast.dwFlags) == 1) << "'");
    return (HCF_HIGHCONTRASTON & highContrast.dwFlags) == 1;
}

// JNI

extern "C" JNIEXPORT bool JNICALL Java_io_github_humbleui_jwm_Theme__1nIsHighContrast
        (JNIEnv* env, jclass jclass) {
    jwm::AppWin32& app = jwm::AppWin32::getInstance();
    jwm::ThemeWin32& theme = app.getTheme();
    return theme.isHighContrast();
}
