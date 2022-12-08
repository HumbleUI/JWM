package io.github.humbleui.jwm;

import java.io.*;
import java.util.concurrent.*;
import java.util.function.*;
import org.jetbrains.annotations.*;
import io.github.humbleui.types.*;

public class WindowMac extends Window {
    @ApiStatus.Internal public float _lastProgressBarValue = -1f;

    @ApiStatus.Internal
    public WindowMac() {
        super(_nMake());
    }

    @Override
    public Window setTextInputEnabled(boolean enabled) {
        assert _onUIThread();
        // TODO: impl me
        return this;
    }

    @Override
    public void unmarkText() {
        assert _onUIThread();
        // TODO: impl me!
    }

    @Override 
    public IRect getWindowRect() {
        assert _onUIThread();
        return _nGetWindowRect();
    }

    @Override 
    public IRect getContentRect() {
        assert _onUIThread();
        return _nGetContentRect();
    }

    @Override
    public Window setWindowPosition(int left, int top) {
        assert _onUIThread();
        if (_nSetWindowPosition(left, top))
            return this;
        else
            throw new IllegalArgumentException("Position is outside of any screen: " + left + ", " + top);
    }

    @Override
    public Window setWindowSize(int width, int height) {
        assert _onUIThread();
        _nSetWindowSize(width, height);
        return this;
    }

    @Override
    public Window setContentSize(int width, int height) {
        assert _onUIThread();
        _nSetContentSize(width, height);
        return this;
    }

    @Override
    public Window setTitle(String title) {
        assert _onUIThread();
        _nSetTitle(title);
        return this;
    }

    /**
     * Hide the title from the title bar without changing the text content.
     *
     * @param isVisible visibility flag value
     * @return this
     */
    @NotNull @Contract("-> this")
    public Window setTitleVisible(boolean isVisible) {
        assert _onUIThread();
        _nSetTitleVisible(isVisible);
        return this;
    }

    @NotNull @Contract("-> this")
    public Window setSubtitle(@NotNull String title) {
        assert _onUIThread();
        _nSetSubtitle(title);
        return this;
    }

    /**
     * <p>Adds the system icon for the specified file next to the window title (and allows viewing the parent directories).</p>
     *
     * @param filename A path to a file
     * @return this
     */
    @NotNull @Contract("-> this")
    public Window setRepresentedFilename(@NotNull String filename) {
        assert _onUIThread();
        _nSetRepresentedFilename(filename);
        return this;
    }

    @Override
    public Window setIcon(File icon) {
        assert _onUIThread();
        _nSetIcon(icon.getAbsolutePath().toString());
        return this;
    }

    /**
     * <p>Shortcut for {@link #setTitleVisible(boolean)}, {@link #setFullSizeContentView(boolean)}</p>
     *
     * <p>TODO: Traffic light visibility</p>
     *
     * @param isVisible visibility flag value
     * @return this
     */
    @Override
    public Window setTitlebarVisible(boolean isVisible) {
        assert _onUIThread();
        setTitleVisible(isVisible);
        setFullSizeContentView(!isVisible);
        setTrafficLightsVisible(isVisible);
        return this;
    }

    @NotNull @Contract("_ -> this")
    public WindowMac setFullSizeContentView(boolean isFullSizeContentView) {
        assert _onUIThread();
        _nSetFullSizeContentView(isFullSizeContentView);
        accept(new EventWindowResize(this));
        return this;
    }

    @NotNull @Contract("_, -> this")
    public WindowMac setTitlebarStyle(WindowMacTitlebarStyle titlebarStyle) {
        assert _onUIThread();
        _nSetTitlebarStyle(titlebarStyle.ordinal());
        accept(new EventWindowResize(this));
        return this;
    }

    @NotNull @Contract("_, _ -> this")
    public WindowMac setTrafficLightPosition(int left, int top) {
        assert _onUIThread();
        _nSetTrafficLightPosition(left, top);
        return this;
    }

    @NotNull @Contract("_ -> this")
    public WindowMac setTrafficLightsVisible(boolean isVisible) {
        assert _onUIThread();
        _nSetTrafficLightsVisible(isVisible);
        return this;
    }

    @ApiStatus.Internal @Override
    public native void _nSetMouseCursor(int cursorIdx);

    @Override
    public Window hideMouseCursorUntilMoved(boolean value) {
        assert _onUIThread();
        _nHideMouseCursorUntilMoved(value);
        return this;
    }

    @Override
    public Window lockMouseCursor(boolean value) {
        assert _onUIThread();
        _nLockMouseCursor(value);
        return this;
    }

    @Override
    public Window setVisible(boolean value) {
        assert _onUIThread();
        _nSetVisible(value);
        return super.setVisible(true);
    }

    @Override
    public Window setOpacity(float opacity) {
        throw new UnsupportedOperationException("impl me!");
    }

    @Override
    public float getOpacity() {
        return 1f;
    }

    @Override
    public Screen getScreen() {
        assert _onUIThread();
        return _nGetScreen();
    }

    @Override
    public void requestFrame() {
        if (!isClosed()) {
            App.runOnUIThread(() -> {
                if (!isClosed()) {
                    _nRequestFrame();
                }
            });
        }
    }

    @Override
    public Window maximize() {
        assert _onUIThread();
        _nMaximize();
        return this;
    }

    @Override
    public Window minimize() {
        assert _onUIThread();
        _nMinimize();
        return this;
    }

    @Override
    public Window restore() {
        assert _onUIThread();
        _nRestore();
        return this;
    }

    @Override
    public Window setFullScreen(boolean value) {
        assert _onUIThread();
        _nSetFullScreen(value);
        return this;
    }

    @Override
    public boolean isFullScreen() {
        assert _onUIThread();
        return _nIsFullScreen();
    }

    @Override
    public Window focus() {
        assert _onUIThread();
        _nFocus();
        return this;
    }

    @Override
    public ZOrder getZOrder() {
        assert _onUIThread();
        return ZOrder._values[_nGetZOrder()];
    }

    @Override
    public Window setZOrder(ZOrder order) {
        assert _onUIThread();
        _nSetZOrder(order.ordinal());
        return this;
    }

    @Override
    public float getProgressBar() {
        assert _onUIThread();
        return _lastProgressBarValue;
    }

    @Override
    public Window setProgressBar(float progress) {
        assert _onUIThread();
        _nSetProgressBar(progress);
        _lastProgressBarValue = progress;
        return this;
    }

    @Override
    public void close() {
        assert _onUIThread() && !isClosed();
        _nClose();
        super.close();
    }

    @ApiStatus.Internal public static native long _nMake();
    @ApiStatus.Internal public native IRect _nGetWindowRect();
    @ApiStatus.Internal public native IRect _nGetContentRect();
    @ApiStatus.Internal public native boolean _nSetWindowPosition(int left, int top);
    @ApiStatus.Internal public native void _nSetWindowSize(int width, int height);
    @ApiStatus.Internal public native void _nSetContentSize(int width, int height);
    @ApiStatus.Internal public native void _nSetTitle(String title);
    @ApiStatus.Internal public native void _nSetTitleVisible(boolean value);
    @ApiStatus.Internal public native void _nSetSubtitle(String title);
    @ApiStatus.Internal public native void _nSetRepresentedFilename(String filename);
    @ApiStatus.Internal public native void _nSetIcon(String path);
    @ApiStatus.Internal public native void _nSetFullSizeContentView(boolean value);
    @ApiStatus.Internal public native void _nSetTitlebarStyle(int titlebarStyle);
    @ApiStatus.Internal public native void _nSetTrafficLightPosition(int left, int top);
    @ApiStatus.Internal public native void _nSetTrafficLightsVisible(boolean value);
    @ApiStatus.Internal public native void _nHideMouseCursorUntilMoved(boolean value);
    @ApiStatus.Internal public native void _nLockMouseCursor(boolean value);
    @ApiStatus.Internal public native void _nSetVisible(boolean value);
    @ApiStatus.Internal public native Screen _nGetScreen();
    @ApiStatus.Internal public native void _nRequestFrame();
    @ApiStatus.Internal public native void _nMinimize();
    @ApiStatus.Internal public native void _nMaximize();
    @ApiStatus.Internal public native void _nRestore();
    @ApiStatus.Internal public native void _nSetFullScreen(boolean value);
    @ApiStatus.Internal public native boolean _nIsFullScreen();
    @ApiStatus.Internal public native void _nFocus();
    @ApiStatus.Internal public native int _nGetZOrder();
    @ApiStatus.Internal public native void _nSetZOrder(int zOrder);
    @ApiStatus.Internal public native void _nSetProgressBar(float value);
    @ApiStatus.Internal public native void _nClose();
}
