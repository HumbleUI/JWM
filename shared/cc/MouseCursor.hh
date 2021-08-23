#pragma once

namespace jwm {

    // Keep sync with MouseCursor.java
    enum class MouseCursor {
        ARROW         = 0,
        CROSSHAIR     = 1,
        HELP          = 2,
        POINTING_HAND = 3,
        IBEAM         = 4,
        UPARROW       = 5,
        NOT_ALLOWED   = 6,
        WAIT          = 7
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
            case MouseCursor::UPARROW:
                return "UpArrow";
            case MouseCursor::NOT_ALLOWED:
                return "NotAllowed";
            case MouseCursor::WAIT:
                return "Wait";
            default:
                return "Unknown";
        }
    }

}