package org.jetbrains.jwm;

import java.util.function.*;
import lombok.*;
import org.jetbrains.annotations.*;

@ToString @EqualsAndHashCode
public class Window implements AutoCloseable {
    static { Library.staticLoad(); }

    public final long _ptr;

    public Window() {
        long ptr = _nCreate(this);
        if (ptr == 0)
            throw new IllegalArgumentException("org.jetbrains.jwm.Window: Can't wrap nullptr");
        _ptr = ptr;
    }

    @Override
    public void close() {
        _nClose(_ptr);
    }

    public Window show() {
        _nShow(_ptr);
        return this;
    }

    @ApiStatus.OverrideOnly
    public void onEvent(Event e) {
    }

    @ApiStatus.Internal public static native long _nCreate(Window window);
    @ApiStatus.Internal public static native void _nClose(long ptr);
    @ApiStatus.Internal public static native void _nShow(long ptr);
}