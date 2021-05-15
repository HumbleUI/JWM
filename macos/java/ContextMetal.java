package org.jetbrains.jwm;

import java.util.function.*;
import lombok.*;
import org.jetbrains.annotations.*;
import org.jetbrains.jwm.*;
import org.jetbrains.jwm.impl.*;

public class ContextMetal extends Context {
    public ContextMetal() {
        super(_nMake());
    }

    public native long getDevicePtr();
    public native long getQueuePtr();
    public native long nextDrawableTexturePtr();

    @ApiStatus.Internal public static native long _nMake();
}