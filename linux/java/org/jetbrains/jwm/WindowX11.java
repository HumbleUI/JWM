package org.jetbrains.jwm;

import java.util.concurrent.*;
import java.util.function.*;
import lombok.*;
import org.jetbrains.annotations.*;
import org.jetbrains.jwm.*;
import org.jetbrains.jwm.impl.*;

public class WindowX11 extends Window {
    @ApiStatus.Internal
    public WindowX11() {
        super(_nMake());
    }

    @Override
    public void show() {
        assert _onUIThread();
        _nShow();
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
    public UIRect getWindowRect() {
        assert _onUIThread();
        return _nGetWindowRect();
    }

    @Override 
    public UIRect getContentRect() {
        assert _onUIThread();
        return _nGetContentRect();
    }

    @Override
    public Window setWindowPosition(int left, int top) {
        assert _onUIThread();
        _nSetWindowPosition(left, top);
        return this;
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
        // TODO: impl me!
        return this;
    }

    @Override
    public Window setMouseCursor(MouseCursor cursor) {
        assert _onUIThread();
        // TODO: impl me!
        return this;
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
    public void close() {
        assert _onUIThread();
        _nClose();
        super.close();
    }

    @Override
    public Window maximize() {
        _nMaximize();
        return this;
    }

    @Override
    public Window minimize() {
        _nMinimize();
        return this;
    }

    @Override
    public Window restore() {
        _nRestore();
        return this;
    }

    @ApiStatus.Internal public static native long _nMake();
    @ApiStatus.Internal public native void _nShow();
    @ApiStatus.Internal public native UIRect _nGetWindowRect();
    @ApiStatus.Internal public native UIRect _nGetContentRect();
    @ApiStatus.Internal public native void _nSetWindowPosition(int left, int top);
    @ApiStatus.Internal public native void _nSetWindowSize(int width, int height);
    @ApiStatus.Internal public native void _nSetContentSize(int width, int height);
    @ApiStatus.Internal public native Screen _nGetScreen();
    @ApiStatus.Internal public native void _nRequestFrame();
    @ApiStatus.Internal public native void _nClose();
    @ApiStatus.Internal public native void _nMaximize();
    @ApiStatus.Internal public native void _nMinimize();
    @ApiStatus.Internal public native void _nRestore();
}