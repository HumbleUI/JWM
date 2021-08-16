package org.jetbrains.jwm;

import org.jetbrains.annotations.*;

public enum LogLevel {
    DEBUG    (0),
    WARNING  (1),
    ERROR    (2),
    FATAL    (3);

    @ApiStatus.Internal public final int _id;
    @ApiStatus.Internal public static final LogLevel[] _values = values();

    @ApiStatus.Internal public static LogLevel makeFromInt(int v) {
        return _values[v];
    }

    @ApiStatus.Internal private LogLevel(int id) {
        _id = id;
    }

    @ApiStatus.Internal public int _getNativeId() {
        return _id;
    }
}