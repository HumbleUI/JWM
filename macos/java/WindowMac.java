package io.github.humbleui.jwm;

import java.io.*;
import java.util.concurrent.*;
import java.util.function.*;
import org.jetbrains.annotations.*;
import io.github.humbleui.types.*;

public class WindowMac extends Window {
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

    @Override
    public Window setIcon(File icon) {
        assert _onUIThread();
        _nSetIcon(icon.getAbsolutePath().toString());
        return this;
    }

    
    @ApiStatus.Internal @Override
    public native void _nSetMouseCursor(int cursorIdx);

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
        assert _onUIThread();
        _nRequestFrame();
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
    public void close() {
        assert _onUIThread();
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
    @ApiStatus.Internal public native void _nSetIcon(String path);
    @ApiStatus.Internal public native void _nSetVisible(boolean value);
    @ApiStatus.Internal public native Screen _nGetScreen();
    @ApiStatus.Internal public native void _nRequestFrame();
    @ApiStatus.Internal public native void _nMinimize();
    @ApiStatus.Internal public native void _nMaximize();
    @ApiStatus.Internal public native void _nRestore();
    @ApiStatus.Internal public native int _nGetZOrder();
    @ApiStatus.Internal public native void _nSetZOrder(int zOrder);
    @ApiStatus.Internal public native void _nClose();
}
