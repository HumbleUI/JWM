#include "ThemeHelperWin32.hh"

#include <Uxtheme.h>
#include <VersionHelpers.h>
#include <WinUser.h>
#include <stdio.h>

#include <Log.hh>
#include <PlatformWin32.hh>

#include "ThemeHelper.hh"

jwm::Theme jwm::ThemeHelperWin32::getCurrentTheme() {
    if (!_isDarkModeSupported()) {
        JWM_VERBOSE("Dark mode is not supported.");
        return jwm::Theme::Light;
    }
    HMODULE hUxTheme = LoadLibrary(TEXT("uxtheme.dll"));
    if (!hUxTheme) {
        JWM_VERBOSE("uxtheme.dll not found.");
        return jwm::Theme::Light;
    }
    jwm::ShouldAppsUseDarkMode hFunc;
    hFunc =
        (ShouldAppsUseDarkMode)GetProcAddress(hUxTheme, MAKEINTRESOURCEA(132));
    if (hFunc == nullptr) {
        JWM_VERBOSE("shouldAppsUseDarkMode not found.");
        FreeLibrary(hUxTheme);
        return jwm::Theme::Light;
    }
    bool shouldAppsUseDarkMode;
    shouldAppsUseDarkMode = hFunc();
    FreeLibrary(hUxTheme);
    return shouldAppsUseDarkMode ? jwm::Theme::Dark : jwm::Theme::Light;
}

jwm::Theme jwm::ThemeHelperWin32::setTheme(jwm::Theme theme) {
    bool isDarkMode =
        (theme == jwm::Theme::Dark) ? _isDarkModeSupported() : false;

    LPCWSTR themeName = isDarkMode ? L"DarkMode_Explorer" : L"";

    HRESULT status = SetWindowTheme(_hWnd, themeName, nullptr);
    if (status != 0) {
        JWM_VERBOSE("Failed to SetWindowTheme");
        return getCurrentTheme();
    }
    bool isOk = _setThemeInternal(_hWnd, theme, isDarkMode);
    // It would be better to reset window theme if this fails.
    return isOk ? theme : getCurrentTheme();
}

bool jwm::ThemeHelperWin32::_setThemeInternal(HWND hWnd, Theme theme,
                                              bool isDarkMode) {
    HMODULE hMod;
    hMod = LoadLibrary(TEXT("user32.dll"));
    if (!hMod) {
        JWM_VERBOSE("Failed to get module handle for user32.dll");
        return false;
    }

    jwm::SetWindowCompositionAttribute fn;
    fn = (jwm::SetWindowCompositionAttribute)GetProcAddress(
        hMod, "SetWindowCompositionAttribute");
    if (!fn) {
        JWM_VERBOSE(
            "Failed to get proc address for SetWindowCompositionAttribute "
            "function");
        FreeLibrary(hMod);
        return false;
    }
    jwm::WINDOWCOMPOSITIONATTRIBDATA data;
    data.Attrib = WCA_USEDARKMODECOLORS;
    data.pvData = &isDarkMode;
    data.cbData = sizeof(&isDarkMode);

    bool status = fn(hWnd, &data);
    JWM_VERBOSE("SetWindowCompositionAttribute '" << status << "'");
    FreeLibrary(hMod);
    return status != FALSE;
}

// strict version check
bool jwm::ThemeHelperWin32::_checkOSVersion() {
    HMODULE hMod;
    jwm::RtlGetVersion_FUNC func;
    hMod = LoadLibrary(TEXT("ntdll.dll"));
    if (hMod) {
        func = (jwm::RtlGetVersion_FUNC)GetProcAddress(hMod, "RtlGetVersion");
        if (func == 0) {
            JWM_VERBOSE("Failed to load RtlGetVersion function from ntdll.dll");
            FreeLibrary(hMod);
            return false;
        }
        NTSTATUS status;
        OSVERSIONINFOW _osVersionInfo;
        OSVERSIONINFOW *_osVersionInfoPtr = &_osVersionInfo;
        ZeroMemory(_osVersionInfoPtr, sizeof(*_osVersionInfoPtr));
        _osVersionInfoPtr->dwOSVersionInfoSize = sizeof(*_osVersionInfoPtr);
        status = func(&_osVersionInfo);
        FreeLibrary(hMod);
        if (status != 0) {
            JWM_VERBOSE("Failed to get osVersionInfo");
            return false;
        }
        JWM_VERBOSE("get os version: '" << _osVersionInfo.dwMajorVersion << "."
                                        << _osVersionInfo.dwMinorVersion << "-"
                                        << _osVersionInfo.dwBuildNumber << "'");
        return _osVersionInfo.dwMajorVersion == 10 &&
               _osVersionInfo.dwMinorVersion == 0 &&
               _osVersionInfo.dwBuildNumber >= 17763;
    }
    JWM_VERBOSE("ntdll.dll not found");
    return false;
}

bool jwm::ThemeHelperWin32::_isDarkModeSupported() {
    if (_isHicontrast()) {
        return false;
    }
    // NOTE: skip strict OS version check since JWM currently(Sep.2021) targets
    // Windows 10+. Assume if your windows version is 10 or higher, it supports
    // dark mode.
    return IsWindows10OrGreater();
}
bool jwm::ThemeHelperWin32::_isHicontrast() {
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