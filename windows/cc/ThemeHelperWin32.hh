#pragma once
#include "ThemeHelper.hh"
#include "ThemeHelperWin32.hh"
#include <PlatformWin32.hh>

namespace jwm
{
  typedef enum {
    	WCA_USEDARKMODECOLORS = 26
  } WINDOWCOMPOSITIONATTRIB;

  typedef struct {
    WINDOWCOMPOSITIONATTRIB Attrib;
    PVOID pvData;
    SIZE_T cbData;
  } WINDOWCOMPOSITIONATTRIBDATA;

  typedef NTSTATUS(WINAPI *RtlGetVersion_FUNC)(OSVERSIONINFOW *);
  typedef bool(WINAPI *ShouldAppsUseDarkMode)();
  typedef bool(WINAPI *SetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA *);
  

  class ThemeHelperWin32 final : public ThemeHelper
  {
  public:
    explicit ThemeHelperWin32(HWND hWnd){
      _hWnd = hWnd;
    };
    ~ThemeHelperWin32() = default;
    Theme getCurrentTheme();
    Theme setTheme(Theme theme);

  private:
    HWND _hWnd;
    bool _isDarkModeSupported();
    bool _isHicontrast();
    bool _setThemeInternal(HWND hWnd, Theme theme,bool isDarkMode);
  };
}