package org.jetbrains.jwm;

import java.util.concurrent.*;
import java.util.function.*;
import lombok.*;
import org.jetbrains.annotations.*;
import org.jetbrains.jwm.*;
import org.jetbrains.jwm.impl.*;

public class WindowWin32 extends Window {
    @ApiStatus.Internal
    public WindowWin32() {
        super(_nMake());
    }

    @Override
    public Window setTextInputEnabled(boolean enabled) {
        assert _isValidCall();
        _nSetTextInputEnabled(enabled);
        return this;
    }

    @Override
    public void unmarkText() {
        assert _isValidCall();
        _nUnmarkText();
    }

    @Override
    public void show() {
        assert _isValidCall();
        _nShow();
    }

    @Override 
    public UIRect getWindowRect() {
        assert _isValidCall();
        return _nGetWindowRect();
    }

    @Override 
    public UIRect getContentRect() {
        assert _isValidCall();
        return _nGetContentRect();
    }

    @Override
    public Window setWindowPosition(int left, int top) {
        assert _isValidCall();
        _nSetWindowPosition(left, top);
        return this;
    }

    @Override
    public Window setWindowSize(int width, int height) {
        assert _isValidCall();
        _nSetWindowSize(width, height);
        return this;
    }

    @Override
    public Window setContentSize(int width, int height) {
        assert _isValidCall();
        _nSetContentSize(width, height);
        return this;
    }
    
    @Override
    public float getScale() {
        assert _isValidCall();
        return _nGetScale();
    }

    @Override
    public void requestFrame() {
        assert _isValidCall();
        _nRequestFrame();
    }

    @Override
    public void close() {
        assert _isValidCall();
        _nClose();
        super.close();
    }

    @ApiStatus.Internal public static native long _nMake();
    @ApiStatus.Internal public native void _nSetTextInputEnabled(boolean enabled);
    @ApiStatus.Internal public native void _nUnmarkText();
    @ApiStatus.Internal public native void _nShow();
    @ApiStatus.Internal public native UIRect _nGetWindowRect();
    @ApiStatus.Internal public native UIRect _nGetContentRect();
    @ApiStatus.Internal public native void _nSetWindowPosition(int left, int top);
    @ApiStatus.Internal public native void _nSetWindowSize(int width, int height);
    @ApiStatus.Internal public native void _nSetContentSize(int width, int height);
    @ApiStatus.Internal public native float _nGetScale();
    @ApiStatus.Internal public native void _nRequestFrame();
    @ApiStatus.Internal public native void _nClose();
}