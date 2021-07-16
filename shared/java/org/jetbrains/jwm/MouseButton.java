package org.jetbrains.jwm;

import org.jetbrains.annotations.*;

public enum MouseButton {

    PRIMARY,
    MIDDLE,
    SECONDARY,
    BACK,
    FORWARD;


    @ApiStatus.Internal public static final MouseButton[] _values = values();

    public static MouseButton makeFromInt(int v) {
        return _values[v];
    }
}