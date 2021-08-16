package org.jetbrains.jwm;

import org.jetbrains.annotations.*;

public enum LogLevel {
    /**
     * <p>Debug message log level.</p>
     * <p>Used for debug/development, for displaying debug information about the library.</p>
     *
     * <p>Disabled by default in release builds.</p>
     */
    DEBUG(0),

    /**
     * <p>Warning message log level.</p>
     * <p>Used for displaying warning messages in debug/development modes.</p>
     *
     * <p>Disabled by default in release builds.</p>
     */
    WARNING(1),

    /**
     * <p>Error message log level.</p>
     * <p>Used for displaying non-fatal errors in development/debug/release builds.
     * This kind of error can appear primary because of invalid/incorrect input or
     * command sequence from the user. This types of errors must be handled and
     * system must recover after this with no side effects.</p>
     *
     * <p>Enabled by default in release builds</p>
     */
    ERROR(2),

    /**
     * <p>Fatal error message log level.</p>
     * <p>Used for displaying fatal errors in development/debug/release builds.
     * This is kind of error which leads to application crash, no recovery is possible.</p>
     *
     * <p>Enabled by default in release builds.</p>
     */
    FATAL(3);

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