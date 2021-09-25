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
    WIN_UPARROW;

    @ApiStatus.Internal public static final MouseCursor[] _values = values();

    public static MouseCursor makeFromInt(int v) {
        return _values[v];
    }
}