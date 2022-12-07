package io.github.humbleui.jwm;

import org.jetbrains.annotations.*;

// must be kept in sync with TouchType.hh
public enum TouchType {
    DIRECT,   // a direct touch from a user’s finger on a screen
    INDIRECT; // an indirect touch that is not on a screen (e.g. trackpad)

    @ApiStatus.Internal public static final TouchType[] _values = values();

    public static TouchType makeFromInt(int v) {
        return _values[v];
    }
}
