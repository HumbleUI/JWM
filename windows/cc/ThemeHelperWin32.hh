#pragma once
#include <PlatformWin32.hh>

#include "ThemeHelper.hh"
#include "ThemeHelperWin32.hh"

namespace jwm {
    typedef enum { WCA_USEDARKMODECOLORS = 26 } WINDOWCOMPOSITIONATTRIB;

    typedef struct {
        WINDOWCOMPOSITIONATTRIB Attrib;
        PVOID pvData;
        SIZE_T cbData;
    } WINDOWCOMPOSITIONATTRIBDATA;

    typedef bool(WINAPI *FnShouldAppsUseDarkMode)();
    typedef bool(WINAPI *FnSetWindowCompositionAttribute)(
        HWND, WINDOWCOMPOSITIONATTRIBDATA *);

    class ThemeHelperWin32 final : public ThemeHelper {
       public:
           ThemeHelperWin32(HWND hWnd) { _hWnd = hWnd; };
           ThemeHelperWin32() = default;
           ~ThemeHelperWin32() = default;
           Theme getCurrentTheme();
           Theme setTheme(Theme theme);
            // If screen is high contrast mode, disable dark mode.
           bool isHighContrast();

       private:
           HWND _hWnd;
           bool _checkOSVersion();
           // check if OS is Windows 10 and build version >= 17763.
           bool _isDarkModeSupported();
           // This function sets window's title bar light/dark.
           bool _setThemeInternal(HWND hWnd, Theme theme, bool isDarkMode);
    };
}  // namespace jwm
