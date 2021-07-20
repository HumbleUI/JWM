package org.jetbrains.jwm;

import org.jetbrains.annotations.*;

public enum MouseButton {
    PRIMARY   (1 << 0),
    SECONDARY (1 << 1),
    MIDDLE    (1 << 2),
    BACK      (1 << 3),
    FORWARD   (1 << 4);

    @ApiStatus.Internal public final int _mask;

    private MouseButton(int mask) {
        _mask = mask;
    }

    @ApiStatus.Internal public static final MouseButton[] _values = values();

    public static MouseButton makeFromInt(int v) {
        return _values[Integer.numberOfTrailingZeros(v)];
    }
}