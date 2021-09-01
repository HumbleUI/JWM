#pragma once

namespace jwm {
    enum class Theme { Light, Dark };

    class ThemeHelper {
       public:
           Theme getCurrentTheme();
           Theme setTheme(Theme theme);
    };
}  // namespace jwm