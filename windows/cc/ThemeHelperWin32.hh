#pragma once
#include "ThemeHelper.hh"
#include "ThemeHelperWin32.hh"
#include <PlatformWin32.hh>

namespace jwm {
  typedef NTSTATUS (WINAPI * RtlGetVersion_FUNC) (OSVERSIONINFOW *);
  typedef bool(WINAPI *ShouldAppsUseDarkMode)();
  class ThemeHelperWin32 final : public ThemeHelper
  {
  public:
    ThemeHelperWin32() = default;
    ~ThemeHelperWin32() = default;
    Theme getCurrentTheme();
    void setTheme(Theme theme);

  private:
    bool _isDarkModeSupported();

  };
}