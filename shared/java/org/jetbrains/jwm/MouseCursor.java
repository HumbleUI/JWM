package org.jetbrains.jwm;

import org.jetbrains.annotations.*;

public enum MouseCursor {
    ARROW  (0),
    CROSS  (1),
    HAND   (2),
    HELP   (3),
    IBEAM  (4),
    UPARROW(5);

    @ApiStatus.Internal public final int _id;

    private MouseCursor(int id) {
        _id = id;
    }
}