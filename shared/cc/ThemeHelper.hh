#pragma once

namespace jwm {
    // must be kept in sync with Theme.java
    enum class Theme { System, Light, Dark };

    class ThemeHelper {
       public:
           Theme getCurrentTheme();
           Theme setTheme(Theme theme);
           bool isHighContrast();
    };
}  // namespace jwm