#pragma once

namespace jwm {

    // Keep sync with MouseCursor.java
    enum class MouseCursor {
        ARROW         = 0,
        CROSSHAIR     = 1,
        HELP          = 2,
        HAND_POINTING = 3,
        HAND_OPEN     = 4,
        HAND_CLOSED   = 5,
        IBEAM         = 6,
        UPARROW       = 7,
        NOT_ALLOWED   = 8,
        WAIT          = 9
    };

    static const char* mouseCursorToStr(MouseCursor cursor) {
        switch (cursor) {
            case MouseCursor::ARROW:
                return "Arrow";
            case MouseCursor::CROSSHAIR:
                return "Crosshair";
            case MouseCursor::HELP:
                return "Help";
            case MouseCursor::HAND_POINTING:
                return "HandPointing";
            case MouseCursor::HAND_OPEN:
                return "HandOpen";
            case MouseCursor::HAND_CLOSED:
                return "HandClosed";
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