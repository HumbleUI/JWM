package org.jetbrains.jwm;

import org.jetbrains.annotations.*;

public class Log {
    public static boolean enabled() {
        return _nEnabled();
    }

    public static void enable(boolean enabled) {
        _nEnable(enabled);
    }

    public static void setLevel(LogLevel level) {
        _nSetLevel(level._getNativeId());
    }

    public static LogLevel getLevel() {
        return LogLevel.makeFromInt(_nGetLevel());
    }

    @ApiStatus.Internal public static native boolean _nEnabled();
    @ApiStatus.Internal public static native void _nEnable(boolean enabled);
    @ApiStatus.Internal public static native void _nSetLevel(int level);
    @ApiStatus.Internal public static native int _nGetLevel();
}