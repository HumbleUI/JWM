package org.jetbrains.jwm;

import java.io.*;
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
        assert _onUIThread();
        _nSetTextInputEnabled(enabled);
        return this;
    }

    @Override
    public void unmarkText() {
        assert _onUIThread();
        _nUnmarkText();
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
    public Theme getCurrentTheme() {
        assert _onUIThread();
        int nTheme = _nGetCurrentTheme();
        return Theme.makeFromInt(nTheme);
    }

    @Override
    public Window setTitle(String title) {
        assert _onUIThread();
        _nSetTitle(title);
        return this;
    }

    
    @Override
    public Window setIcon(File icon){
        assert _onUIThread();
        _nSetIcon(icon.getAbsolutePath().toString());
        return this;
    }

    @Override
    public Window setVisible(boolean value) {
        assert _onUIThread();
        _nSetVisible(value);
        return this;
    }

    @Override
    public Window setOpacity(float opacity) {
        assert _onUIThread();
        _nSetOpacity(opacity);
        return this;
    }
    
    @Override
    public float getOpacity() {
        assert _onUIThread();
        return _nGetOpacity();
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

    public Window maximize() {
        assert _onUIThread();
        _nMaximize();
        return this;
    }

    public Window minimize() {
        assert _onUIThread();
        _nMinimize();
        return this;
    }

    public Window restore() {
        assert _onUIThread();
        _nRestore();
        return this;
    }

    @Override
    public void close() {
        assert _onUIThread();
        _nClose();
        super.close();
    }

    @ApiStatus.Internal public static native long _nMake();
    @ApiStatus.Internal public native void _nSetTextInputEnabled(boolean enabled);
    @ApiStatus.Internal public native void _nUnmarkText();
    @ApiStatus.Internal public native UIRect _nGetWindowRect();
    @ApiStatus.Internal public native UIRect _nGetContentRect();
    @ApiStatus.Internal public native void _nSetWindowPosition(int left, int top);
    @ApiStatus.Internal public native void _nSetWindowSize(int width, int height);
    @ApiStatus.Internal public native void _nSetContentSize(int width, int height);
    @ApiStatus.Internal public native int _nGetCurrentTheme();
    @ApiStatus.Internal public native void _nSetTitle(String title);
    @ApiStatus.Internal public native void _nSetIcon(String iconPath);
    @ApiStatus.Internal public native void _nSetVisible(boolean value);
    @ApiStatus.Internal public native void _nSetOpacity(float opacity);
    @ApiStatus.Internal public native float _nGetOpacity();
    @ApiStatus.Internal public native void _nSetMouseCursor(int cursorId);
    @ApiStatus.Internal public native Screen _nGetScreen();
    @ApiStatus.Internal public native void _nRequestFrame();
    @ApiStatus.Internal public native void _nMaximize();
    @ApiStatus.Internal public native void _nMinimize();
    @ApiStatus.Internal public native void _nRestore();
    @ApiStatus.Internal public native void _nClose();
}