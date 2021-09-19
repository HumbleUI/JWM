#include "ThemeHelperWin32.hh"

#include <Uxtheme.h>
#include <VersionHelpers.h>
#include <WinUser.h>
#include <stdio.h>
#include <Log.hh>
#include <PlatformWin32.hh>
#include "ThemeHelper.hh"


bool jwm::ThemeHelperWin32::isHighContrast() {
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
