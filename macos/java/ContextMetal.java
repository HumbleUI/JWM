package org.jetbrains.jwm.macos;

import java.util.function.*;
import lombok.*;
import org.jetbrains.annotations.*;
import org.jetbrains.jwm.*;
import org.jetbrains.jwm.impl.*;

public class ContextMetal extends Context {
    public ContextMetal(boolean vsync) {
        super(_nMake(vsync));
    }

    public native long getDevicePtr();
    public native long getQueuePtr();
    public native long nextDrawableTexturePtr();
    public native int getWidth();
    public native int getHeight();
 
    @ApiStatus.Internal public static native long _nMake(boolean vsync);
}