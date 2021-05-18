package org.jetbrains.jwm;

import java.lang.ref.*;
import java.util.function.*;
import lombok.*;
import org.jetbrains.annotations.*;
import org.jetbrains.jwm.impl.*;

public abstract class Window extends RefCounted {
    @ApiStatus.Internal
    public Layer _layer = null;

    @ApiStatus.Internal
    public Consumer<Event> _eventListener = null;

    @ApiStatus.Internal
    public Window(long ptr) {
        super(ptr);
    }

    @NotNull @Contract("-> this")
    public native Window setEventListener(@Nullable Consumer<Event> eventListener);

    @NotNull @Contract("-> this")
    public Window attach(@Nullable Layer layer) {
        if (_layer != null) {
            _layer._nDetach();
            _layer = null;
        }
        
        if (layer != null)
            layer._nAttach(this);

        _layer = layer;
        
        return this;
    }

    @NotNull @Contract("-> this")
    public abstract Window show();

    @Override
    public void close() {
        if (_layer != null) {
            _layer._nDetach();
            _layer = null;
        }
        super.close();
        App._windows.remove(this);
        Reference.reachabilityFence(_eventListener);
    }
}