#pragma once

namespace jwm {
    // must be kept in sync with TouchType.java
    enum class TouchType {
        DIRECT,   // a direct touch from a user’s finger on a screen
        INDIRECT, // an indirect touch that is not on a screen (e.g. trackpad)
    };
}
