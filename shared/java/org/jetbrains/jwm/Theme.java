package org.jetbrains.jwm;

import org.jetbrains.annotations.*;

// must be kept in sync with ThemeHelper.hh
public enum Theme {
    LIGHT,
    DARK;

    @ApiStatus.Internal public static final Theme[] _values = values();

    public static Theme makeFromInt(int v) {
        return _values[v];
    }
}