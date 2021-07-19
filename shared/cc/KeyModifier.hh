#pragma once

namespace jwm {
    // must be kept in sync with KeyModifier.java
    enum class KeyModifier {
        SHIFT    = 1 << 0,
        CONTROL  = 1 << 1,
        ALT      = 1 << 2,
        COMMAND  = 1 << 3,
        WINDOWS  = 1 << 4,
        META     = 1 << 5,
        FUNCTION = 1 << 6,
        CAPS     = 1 << 7
    };
}
