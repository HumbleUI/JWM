package io.github.humbleui.jwm;

import java.util.function.*;
import org.jetbrains.annotations.*;

public class Log {
    @ApiStatus.Internal public static boolean _verbose = false;
    @ApiStatus.Internal public static Consumer<Object> _listener;

    /**
     * <p>Log (default) message to JWM logger.</p>
     * <p>Has no effect if message consumer is not set.</p>
     *
     * @param message       message to log
     */
    public static void log(Object message) {
        assert _onUIThread();
        if (_listener != null)
            _listener.accept(message);
    }

    /**
     * <p>Log verbose message to JWM logger.</p>
     * <p>Has no effect if message consumer is not set or verbose level is not enabled.</p>
     *
     * @param message       message to log
     */
    public static void verbose(Object message) {
        assert _onUIThread();
        if (_listener != null && _verbose)
            _listener.accept(message);
    }

    /**
     * <p>Set JWM application log messages consumer.</p>
     * <p>Accepts default and verbose log messages.</p>
     */
    public static void setLogger(@Nullable Consumer<Object> listener) {
        assert _onUIThread();
        _listener = listener;
        _nSetListener(listener);
    }

    /**
     * <p>Enable verbose log messages.</p>
     * <p>By default, verbose messages are enabled on application init if environ variable JWM_VERBOSE=true is set.</p>
     *
     * @param enabled       flag to enable or disable verbose messages
     */
    public static void setVerbose(boolean enabled) {
        assert _onUIThread();
        _verbose = enabled;
        _nSetVerbose(enabled);
    }
    
    @ApiStatus.Internal public static boolean _onUIThread() {
        return App._onUIThread();
    }

    @ApiStatus.Internal public static native void _nSetVerbose(boolean enabled);
    @ApiStatus.Internal public static native void _nSetListener(Consumer<Object> listener);
}