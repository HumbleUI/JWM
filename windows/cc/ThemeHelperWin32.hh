#pragma once
#include <PlatformWin32.hh>

#include "ThemeHelper.hh"
#include "ThemeHelperWin32.hh"

namespace jwm {
  
    class ThemeHelperWin32 final : public ThemeHelper {
       public:
           ThemeHelperWin32(HWND hWnd) { _hWnd = hWnd; };
           ThemeHelperWin32() = default;
           ~ThemeHelperWin32() = default;
       
           bool isHighContrast();

       private:
           HWND _hWnd;
    };
}  // namespace jwm
