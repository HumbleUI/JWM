package org.jetbrains.jwm;

import java.util.concurrent.*;
import java.util.function.*;
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

    @NotNull @Contract("-> this")
    public native Window setTextInputClient(@Nullable TextInputClient client);

    @NotNull @Contract("-> this")
    public native Window setTextInputEnabled(boolean enabled);

    public void unmarkText() { System.out.println("Window::unmarkText()"); } // TODO abstract

    public abstract void show();

    public abstract UIRect getWindowRect();

    public abstract UIRect getContentRect();

    public abstract float getScale(); // TODO Screen API

    public abstract Window setWindowPosition(int left, int top);

    public abstract Window setWindowSize(int width, int height);
    
    public abstract Window setContentSize(int width, int height);

    public abstract void requestFrame();

    @Override
    public void close() {
        setEventListener(null);
        App._windows.remove(this);
        super.close();
    }
}