package org.jetbrains.jwm;

import java.util.function.*;
import lombok.*;
import org.jetbrains.annotations.*;
import org.jetbrains.jwm.impl.*;

public abstract class Window extends Managed {
    @ApiStatus.Internal
    public static class _FinalizerHolder {
        public static final long PTR = _nGetFinalizer();
    }

    @ApiStatus.Internal
    public Context _context = null;

    @ApiStatus.Internal
    public Consumer<Event> _eventListener =  null;

    @ApiStatus.Internal
    public Window(long ptr) {
        super(ptr, _FinalizerHolder.PTR);
    }

    @NotNull @Contract("-> this")
    public Window setEventListener(@Nullable Consumer<Event> eventListener) {
        _eventListener = eventListener;
        _nSetEventListener(eventListener);
        return this;
    }

    @NotNull @Contract("-> this")
    public Window attach(@Nullable Context context) {
        if (_context != null) {
            _nDetach();
            _context.close();
            _context = null;
        }
        
        if (context != null)
            _nAttach(context);

        _context = context;
        
        return this;
    }

    @NotNull @Contract("-> this")
    public abstract Window show();

    @Override
    public void close() {
        super.close();
        App._windows.remove(this);
    }

    @ApiStatus.Internal public static native long _nGetFinalizer();
    @ApiStatus.Internal public native void _nSetEventListener(Consumer<Event> listener);
    @ApiStatus.Internal public native void _nAttach(Context context);
    @ApiStatus.Internal public native void _nDetach();
}