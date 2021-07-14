package org.jetbrains.jwm;

import java.util.function.*;
import lombok.*;
import org.jetbrains.annotations.*;
import org.jetbrains.jwm.*;
import org.jetbrains.jwm.impl.*;

public class ScreenWin32 extends Screen {
    @ApiStatus.Internal
    public ScreenWin32() {
       super(_nMake());
    }

    @Override
    public native int getX();

    @Override
    public native int getY();

    @Override
    public native int getWidth();

    @Override
    public native int getHeight();

    @Override
    public native float getScale();

    @Override
    public native boolean isPrimary();

    @Override
    public void close() {
        super.close();
    }

    @ApiStatus.Internal public static native long _nMake();
    @ApiStatus.Internal public native void _nClose();
}