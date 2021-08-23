#pragma once

namespace jwm {

    // Keep sync with MouseCursor.java
    enum class MouseCursor {
        ARROW   = 0,
        CROSS   = 1,
        HAND    = 2,
        HELP    = 3,
        IBEAM   = 4,
        UPARROW = 5
    };

    static const char* mouseCursorToStr(MouseCursor cursor) {
        switch (cursor) {
            case MouseCursor::ARROW:
                return "Arrow";
            case MouseCursor::CROSS:
                return "Cross";
            case MouseCursor::HAND:
                return "Hand";
            case MouseCursor::HELP:
                return "Help";
            case MouseCursor::IBEAM:
                return "I-beam";
            case MouseCursor::UPARROW:
                return "UpArrow";
            default:
                return "Unknown";
        }
    }

}