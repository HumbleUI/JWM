package org.jetbrains.jwm;

import java.util.function.*;
import lombok.*;
import org.jetbrains.annotations.*;
import org.jetbrains.jwm.impl.*;

public class Window extends Managed {
    static { Library.staticLoad(); }

    @ApiStatus.Internal
    public static class _FinalizerHolder {
        public static final long PTR = _nGetFinalizer();
    }

    public MetalContext _context = null;

    public Window() {
        super(_nMake(), _FinalizerHolder.PTR);
    }

    @NotNull @Contract("-> this")
    public Window setEventListener(@NotNull Consumer<Event> eventListener) {
        _nSetEventListener(_ptr, eventListener);
        return this;
    }

    @NotNull @Contract("-> this")
    public Window attach(MetalContext context) {
        if (_context != null) {
            context.close();
        }
        _nAttach(_ptr, context._ptr);
        _context = context;
        return this;
    }

    @NotNull @Contract("-> this")
    public Window show() {
        _nShow(_ptr);
        return this;
    }

    @ApiStatus.Internal public static native long _nGetFinalizer();
    @ApiStatus.Internal public static native long _nMake();
    @ApiStatus.Internal public static native void _nSetEventListener(long ptr, Consumer<Event> listener);
    @ApiStatus.Internal public static native void _nAttach(long ptr, long contextPtr);
    @ApiStatus.Internal public static native void _nShow(long ptr);
}