#pragma once

namespace jwm {

    // Keep sync with MouseCursor.java and WindowMac.mm
    enum class MouseCursor {
        ARROW         = 0,
        CROSSHAIR     = 1,
        HELP          = 2,
        POINTING_HAND = 3,
        IBEAM         = 4,
        NOT_ALLOWED   = 5,
        WAIT          = 6,
        WIN_UPARROW   = 7,
        N_RESIZE      = 8,
        E_RESIZE      = 9,
        S_RESIZE      = 10,
        W_RESIZE      = 11,
        NE_RESIZE     = 12,
        NW_RESIZE     = 13,
        SE_RESIZE     = 14,
        SW_RESIZE     = 15,
        EW_RESIZE     = 16,
        NS_RESIZE     = 17,
        NESW_RESIZE   = 18,
        NWSE_RESIZE   = 19,
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
            case MouseCursor::N_RESIZE:
                return "N-resize";
            case MouseCursor::E_RESIZE:
                return "E-resize";
            case MouseCursor::S_RESIZE:
                return "S-resize";
            case MouseCursor::W_RESIZE:
                return "W-resize";
            case MouseCursor::NE_RESIZE:
                return "NE-resize";
            case MouseCursor::NW_RESIZE:
                return "NW-resize";
            case MouseCursor::SE_RESIZE:
                return "SE-resize";
            case MouseCursor::SW_RESIZE:
                return "SW-resize";
            case MouseCursor::EW_RESIZE:
                return "EW-resize";
            case MouseCursor::NS_RESIZE:
                return "NS-resize";
            case MouseCursor::NESW_RESIZE:
                return "NESW-resize";
            case MouseCursor::NWSE_RESIZE:
                return "NWSE-resize";
            default:
                return "Unknown";
        }
    }

}