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
    public native Window setTextInputEnabled(boolean enabled);

    @Override
    public native void unmarkText();

    @Override
    public native void show();

    @Override 
    public UIRect getWindowRect() {
        return _nGetWindowRect();
    }

    @Override 
    public UIRect getContentRect() {
        return _nGetContentRect();
    }

    @Override
    public Window setWindowPosition(int left, int top) {
        _nSetWindowPosition(left, top);
        return this;
    }

    @Override
    public Window setWindowSize(int width, int height) {
        _nSetWindowSize(width, height);
        return this;
    }

    @Override
    public Window setContentSize(int width, int height) {
        _nSetContentSize(width, height);
        return this;
    }
    
    @Override
    public Screen getScreen() {
        return _nGetScreen();
    }

    @Override
    public native void requestFrame();

    @Override
    public void close() {
        _nClose();
        super.close();
    }

    @ApiStatus.Internal public static native long _nMake();
    @ApiStatus.Internal public native UIRect _nGetWindowRect();
    @ApiStatus.Internal public native UIRect _nGetContentRect();
    @ApiStatus.Internal public native void _nSetWindowPosition(int left, int top);
    @ApiStatus.Internal public native void _nSetWindowSize(int width, int height);
    @ApiStatus.Internal public native void _nSetContentSize(int width, int height);
    @ApiStatus.Internal public native Screen _nGetScreen();
    @ApiStatus.Internal public native void _nClose();
}