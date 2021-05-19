package org.jetbrains.jwm;

import java.lang.ref.*;
import java.util.function.*;
import lombok.*;
import org.jetbrains.annotations.*;
import org.jetbrains.jwm.impl.*;

public abstract class Window extends RefCounted {
    @ApiStatus.Internal
    public Consumer<Event> _eventListener = null;

    @ApiStatus.Internal
    public Window(long ptr) {
        super(ptr);
    }

    @NotNull @Contract("-> this")
    public native Window setEventListener(@Nullable Consumer<Event> eventListener);

    public abstract void show();

    public abstract int getLeft();

    public abstract int getTop();

    public abstract int getWidth();

    public abstract int getHeight();

    public abstract float getScale(); // TODO Screen API

    public abstract void move(int left, int top);

    public abstract void resize(int width, int height);

    public abstract void requestFrame();

    @Override
    public void close() {
        setEventListener(null);
        App._windows.remove(this);
        super.close();
    }
}