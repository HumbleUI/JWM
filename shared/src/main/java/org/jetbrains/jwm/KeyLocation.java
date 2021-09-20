package org.jetbrains.jwm;

import org.jetbrains.annotations.*;

// must be kept in sync with KeyLocation.hh
public enum KeyLocation {
    DEFAULT,
    RIGHT,
    KEYPAD;

    @ApiStatus.Internal public static final KeyLocation[] _values = values();

    public static KeyLocation makeFromInt(int v) {
        return _values[v];
    }
}