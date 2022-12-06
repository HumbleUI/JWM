package io.github.humbleui.jwm;

import org.jetbrains.annotations.*;

// must be kept in sync with TouchType.hh
public enum TouchType {
    DIRECT,
    INDIRECT;

    @ApiStatus.Internal public static final TouchType[] _values = values();

    public static TouchType makeFromInt(int v) {
        return _values[v];
    }
}
