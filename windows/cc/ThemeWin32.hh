#pragma once
#include <PlatformWin32.hh>

#include "Theme.hh"
#include "ThemeWin32.hh"

namespace jwm {
  
    class ThemeWin32 final : public Theme {
       public:
           ThemeWin32() = default;
           ~ThemeWin32() = default;
       
           bool isHighContrast();

    };
}  // namespace jwm
