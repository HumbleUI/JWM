#pragma once

namespace jwm {

    // Keep sync with MouseCursor.java
    enum class MouseCursor {
        ARROW         = 0,
        CROSSHAIR     = 1,
        HELP          = 2,
        POINTING_HAND = 3,
        IBEAM         = 4,
        NOT_ALLOWED   = 5,
        WAIT          = 6,
        WIN_UPARROW   = 7,

        // total enum count; keep this at the end
        COUNT,
    };

    static const char* mouseCursorToStr(MouseCursor cursor) {
        switch (cursor) {
            case MouseCursor::ARROW:
                return "Arrow";
            case MouseCursor::CROSSHAIR:
                return "Crosshair";
            case MouseCursor::HELP:
                return "Help";
            case MouseCursor::POINTING_HAND:
                return "PointingHand";
            case MouseCursor::IBEAM:
                return "I-beam";
            case MouseCursor::NOT_ALLOWED:
                return "NotAllowed";
            case MouseCursor::WAIT:
                return "Wait";
            case MouseCursor::WIN_UPARROW:
                return "UpArrow";
            default:
                return "Unknown";
        }
    }

}