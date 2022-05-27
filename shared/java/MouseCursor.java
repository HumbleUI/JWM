package io.github.humbleui.jwm;

import org.jetbrains.annotations.*;

public enum MouseCursor {
    ARROW,
    CROSSHAIR,
    HELP,
    POINTING_HAND,
    IBEAM,
    NOT_ALLOWED,
    WAIT,
    WIN_UPARROW,
    N_RESIZE,
    E_RESIZE,
    S_RESIZE,
    W_RESIZE,
    NE_RESIZE,
    NW_RESIZE,
    SE_RESIZE,
    SW_RESIZE,
    EW_RESIZE,
    NS_RESIZE,
    NESW_RESIZE,
    NWSE_RESIZE;

    @ApiStatus.Internal public static final MouseCursor[] _values = values();

    public static MouseCursor makeFromInt(int v) {
        return _values[v];
    }
}