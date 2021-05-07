package org.jetbrains.jwm;

import java.util.function.*;
import lombok.*;
import org.jetbrains.annotations.*;
import org.jetbrains.jwm.*;
import org.jetbrains.jwm.impl.*;

public class ContextGL extends Context {
    public ContextGL(ContextGLOpts opts) {
        super(_nMake(opts._vsync));
    }

    @ApiStatus.Internal public static native long _nMake(boolean vsync);
}