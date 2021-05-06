package org.jetbrains.jwm;

import java.util.function.*;
import lombok.*;
import org.jetbrains.annotations.*;
import org.jetbrains.jwm.impl.*;

public class Window extends Managed {
    @ApiStatus.Internal
    public static class _FinalizerHolder {
        public static final long PTR = _nGetFinalizer();
    }

    @ApiStatus.Internal
    public Context _context = null;

    @ApiStatus.Internal
    public Consumer<Event> _eventListener =  null;

    @ApiStatus.Internal
    public Window() {
        super(_nMake(), _FinalizerHolder.PTR);
    }

    @NotNull @Contract("-> this")
    public Window setEventListener(@Nullable Consumer<Event> eventListener) {
        _eventListener = eventListener;
        _nSetEventListener(_ptr, eventListener);
        return this;
    }

    @NotNull @Contract("-> this")
    public Window attach(Context context) {
        if (_context != null)
            context.close();
        _nAttach(_ptr, context._ptr);
        _context = context;
        return this;
    }

    @NotNull @Contract("-> this")
    public Window show() {
        _nShow(_ptr);
        return this;
    }

    @Override
    public void close() {
        super.close();
        App._windows.remove(this);
    }

    @ApiStatus.Internal public static native long _nGetFinalizer();
    @ApiStatus.Internal public static native long _nMake();
    @ApiStatus.Internal public static native void _nSetEventListener(long ptr, Consumer<Event> listener);
    @ApiStatus.Internal public static native void _nAttach(long ptr, long contextPtr);
    @ApiStatus.Internal public static native void _nShow(long ptr);
}