package org.jetbrains.jwm;

import java.util.function.*;
import lombok.*;
import org.jetbrains.annotations.*;
import org.jetbrains.jwm.*;
import org.jetbrains.jwm.impl.*;

public class WindowMac extends Window {
    @ApiStatus.Internal
    public WindowMac(WindowOpts opts) {
        super(_nMake());
    }

    @Override @NotNull @Contract("-> this")
    public Window show() {
        _nShow(_ptr);
        return this;
    }

    @ApiStatus.Internal public static native long _nMake();
    @ApiStatus.Internal public native void _nShow(long ptr);
}