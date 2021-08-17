package org.jetbrains.jwm;

import java.util.concurrent.*;
import java.util.function.*;
import org.jetbrains.annotations.*;

public class WindowMac extends Window {
    @ApiStatus.Internal
    public WindowMac() {
        super(_nMake());
    }

    @Override
    public native void show();

    @Override 
    public native int getLeft();

    @Override 
    public native int getTop();

    @Override 
    public native int getWidth();

    @Override 
    public native int getHeight();

    @Override 
    public native float getScale();

    @Override
    public native void move(int left, int top);

    @Override
    public native void resize(int width, int height);

    @Override
    public native void requestFrame();

    @Override
    public void close() {
        _nClose();
        super.close();
    }

    @ApiStatus.Internal public static native long _nMake();
    @ApiStatus.Internal public native void _nClose();
}