package org.jetbrains.jwm;

import java.util.function.*;
import lombok.*;
import org.jetbrains.annotations.*;
import org.jetbrains.jwm.*;
import org.jetbrains.jwm.impl.*;

public class WindowMac extends Window {
    @ApiStatus.Internal
    public WindowMac() {
        super(_nMake());
    }

    @Override @NotNull @Contract("-> this")
    public native Window show();

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

    @Override @NotNull @Contract("-> this")
    public native Window move(int left, int top);

    @Override @NotNull @Contract("-> this")
    public native Window resize(int width, int height);

    @Override
    public void close() {
        _nClose();
        super.close();
    }

    @ApiStatus.Internal public static native long _nMake();
    @ApiStatus.Internal public native void _nClose();
}