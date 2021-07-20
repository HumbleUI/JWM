#pragma once
#include <initializer_list>

namespace jwm {
    // must be kept in sync with MouseButton.java
    enum class MouseButton {
        PRIMARY    = 1 << 0,
        SECONDARY  = 1 << 1,
        MIDDLE     = 1 << 2,
        BACK       = 1 << 3,
        FORWARD    = 1 << 4
    };

    constexpr std::initializer_list<MouseButton> kMouseButtonValues = {
        MouseButton::PRIMARY,
        MouseButton::SECONDARY,
        MouseButton::MIDDLE,
        MouseButton::BACK,
        MouseButton::FORWARD
    };
}