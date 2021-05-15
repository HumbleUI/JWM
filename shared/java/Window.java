package org.jetbrains.jwm;

import java.lang.ref.*;
import java.util.function.*;
import lombok.*;
import org.jetbrains.annotations.*;
import org.jetbrains.jwm.impl.*;

public abstract class Window extends RefCounted {
    @ApiStatus.Internal
    public Context _context = null;

    @ApiStatus.Internal
    public Consumer<Event> _eventListener = null;

    @ApiStatus.Internal
    public Window(long ptr) {
        super(ptr);
    }

    @NotNull @Contract("-> this")
    public native Window setEventListener(@Nullable Consumer<Event> eventListener);

    @NotNull @Contract("-> this")
    public Window attach(@Nullable Context context) {
        if (_context != null) {
            _context._nDetach();
            _context = null;
        }
        
        if (context != null)
            context._nAttach(this);

        _context = context;
        
        return this;
    }

    @NotNull @Contract("-> this")
    public abstract Window show();

    @Override
    public void close() {
        if (_context != null) {
            _context._nDetach();
            _context = null;
        }
        super.close();
        App._windows.remove(this);
        Reference.reachabilityFence(_eventListener);
    }
}