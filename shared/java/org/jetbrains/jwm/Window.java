package org.jetbrains.jwm;

import java.util.concurrent.*;
import java.util.function.*;
import org.jetbrains.annotations.*;
import org.jetbrains.jwm.impl.*;

public abstract class Window extends RefCounted {
    @ApiStatus.Internal
    public Window(long ptr) {
        super(ptr);
    }

    @NotNull @Contract("-> this")
    public Window setEventListener(@Nullable Consumer<Event> eventListener) {
        assert _isValidCall();
        _nSetEventListener(eventListener);
        return this;
    }

    @NotNull @Contract("-> this")
    public Window setTextInputClient(@Nullable TextInputClient client) {
        assert _isValidCall();
        _nSetTextInputClient(client);
        return this;
    }

    @NotNull @Contract("-> this")
    public abstract Window setTextInputEnabled(boolean enabled);

    public abstract void unmarkText();

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
        assert _isValidCall();
        setEventListener(null);
        App._windows.remove(this);
        super.close();
    }

    @ApiStatus.Internal public static boolean _isValidCall() {
        return App._isValidCall();
    }

    @ApiStatus.Internal public native void _nSetEventListener(Consumer<Event> eventListener);
    @ApiStatus.Internal public native void _nSetTextInputClient(TextInputClient client);
}