package org.jetbrains.jwm;

import lombok.*;
import org.jetbrains.annotations.*;
import org.jetbrains.jwm.impl.*;

public abstract class Context extends RefCounted {
    @ApiStatus.Internal
    public Context(long ptr) {
        super(ptr);
    }

    public native long swapBuffers();
    public native void resize();
    public native int getWidth();
    public native int getHeight();
    public native float getScale();

    @ApiStatus.Internal public native void _nAttach(Window window);
    @ApiStatus.Internal public native void _nDetach();
}