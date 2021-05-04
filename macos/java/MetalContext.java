package org.jetbrains.jwm.macos;

import java.util.function.*;
import lombok.*;
import org.jetbrains.annotations.*;
import org.jetbrains.jwm.*;
import org.jetbrains.jwm.impl.*;

public class MetalContext extends Context {
    static { Library.staticLoad(); }

    public MetalContext(boolean vsync) {
        super(_nMake(vsync));
    }

    @ApiStatus.Internal public static native long _nMake(boolean vsync);
}