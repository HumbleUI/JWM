package org.jetbrains.jwm;

import lombok.*;
import org.jetbrains.annotations.*;
import org.jetbrains.jwm.impl.*;

public abstract class Context extends Managed {
    @ApiStatus.Internal
    public static class _FinalizerHolder {
        public static final long PTR = _nGetFinalizer();
    }

    @ApiStatus.Internal
    public Context(long ptr) {
        super(ptr, _FinalizerHolder.PTR);
    }

    public native long swapBuffers();
    public native void resize();
    public native int getWidth();
    public native int getHeight();
    public native float getScale();

    @ApiStatus.Internal public static native long _nGetFinalizer();
}