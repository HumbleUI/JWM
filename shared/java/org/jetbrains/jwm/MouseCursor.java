package org.jetbrains.jwm;

import org.jetbrains.annotations.*;

public enum MouseCursor {
    ARROW         (0),
    CROSSHAIR     (1),
    HELP          (2),
    HAND_POINTING (3),
    HAND_OPEN     (4),
    HAND_CLOSED   (5),
    IBEAM         (6),
    UPARROW       (7),
    NOT_ALLOWED   (8),
    WAIT          (9);

    @ApiStatus.Internal public final int _id;

    @ApiStatus.Internal public static final MouseCursor[] _values = values();

    public static MouseCursor makeFromInt(int v) {
        return _values[v];
    }

    private MouseCursor(int id) {
        _id = id;
    }
}