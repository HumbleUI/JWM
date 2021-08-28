#pragma once

namespace jwm {
  enum class Theme
  {
    Light,
    Dark
  };
  
  class ThemeHelper {
    public:
      Theme getCurrentTheme();
      void setTheme(Theme theme);
  };
}