#pragma once

namespace jwm {
    // must be kept in sync with KeyModifier.java
    enum class KeyModifier {
        SHIFT   = 1 << 0,
        CTRL    = 1 << 1,
        ALT     = 1 << 2,
        CMD     = 1 << 3,
        WINDOWS = 1 << 4,
        META    = 1 << 5;
    };
}
