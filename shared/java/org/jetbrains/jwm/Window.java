package org.jetbrains.jwm;

import java.util.concurrent.*;
import java.util.function.*;
import org.jetbrains.annotations.*;
import org.jetbrains.jwm.impl.*;
import java.io.File;

public abstract class Window extends RefCounted {
    @ApiStatus.Internal
    public Window(long ptr) {
        super(ptr);
    }

    @NotNull @Contract("-> this")
    public Window setEventListener(@Nullable Consumer<Event> eventListener) {
        assert _onUIThread();
        _nSetEventListener(eventListener);
        return this;
    }

    @NotNull @Contract("-> this")
    public Window setTextInputClient(@Nullable TextInputClient client) {
        assert _onUIThread();
        _nSetTextInputClient(client);
        return this;
    }

    @NotNull @Contract("-> this")
    public abstract Window setTextInputEnabled(boolean enabled);

    public abstract void unmarkText();

    public abstract void show();

    public abstract UIRect getWindowRect();

    public abstract UIRect getContentRect();

    public UIRect getContentRectAbsolute() {
        UIRect windowRect = getWindowRect();
        return getContentRect().offset(windowRect._left, windowRect._top);
    }

    @NotNull @Contract("-> this")
    public abstract Window setWindowPosition(int left, int top);

    @NotNull @Contract("-> this")
    public abstract Window setWindowSize(int width, int height);
    
    @NotNull @Contract("-> this")
    public abstract Window setContentSize(int width, int height);

    @NotNull @Contract("-> this")
    public abstract Window setTitle(String title);

    @NotNull @Contract("-> this")
    public abstract Window setIcon(File icon);
    public abstract Window setMouseCursor(MouseCursor cursor);
    
    /**
    *  Sets window opacity (0 - 255)
     */
    @NotNull @Contract("-> this")
    public abstract Window setOpacity(int opacity);

    public abstract Screen getScreen();

    public abstract void requestFrame();

    /**
     * Expands the window to take up all of the space of the desktop except taskbars, docks, etc..
     * On Windows titlebar, the single square icon [] has the same behaviour.
     * On macOS titlebar, the yellow circle has the same behaviour.
     */
    public abstract Window maximize();

    /**
     * Hides the window to the system taskbar or dock.
     * On Windows titlebar, the underline icon _ has the same behaviour.
     * On macOS titlebar, the green circle has the same behaviour.
     */
    public abstract Window minimize();

    /**
     * Cancels the effect of Window::minimize() and Window::maximize() functions.
     * On Windows titlebar, the double square icon []] has the same behaviour.
     * On macOS titlebar, the yellow circle has the same behaviour.
     */
    public abstract Window restore();

    @Override
    public void close() {
        assert _onUIThread();
        setEventListener(null);
        App._windows.remove(this);
        super.close();
    }

    @ApiStatus.Internal public static boolean _onUIThread() {
        return App._onUIThread();
    }

    @ApiStatus.Internal public native void _nSetEventListener(Consumer<Event> eventListener);
    @ApiStatus.Internal public native void _nSetTextInputClient(TextInputClient client);
}