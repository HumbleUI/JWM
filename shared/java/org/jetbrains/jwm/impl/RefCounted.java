package org.jetbrains.jwm.impl;

import java.lang.ref.*;
import org.jetbrains.annotations.*;

public abstract class RefCounted extends Managed {
    @ApiStatus.Internal
    public RefCounted(long ptr) {
        super(ptr, _FinalizerHolder.PTR);
    }

    @ApiStatus.Internal
    public RefCounted(long ptr, boolean allowClose) {
        super(ptr, _FinalizerHolder.PTR, allowClose);
    }

    public native int getRefCount();

    @Override
    public String toString() {
        String s = super.toString();
        try {
            return s.substring(0, s.length() - 1) + ", refCount=" + getRefCount() + ")";
        } catch (Throwable t) {
            return s;
        }
    }

    @ApiStatus.Internal
    public static class _FinalizerHolder {
        public static final long PTR = _nGetFinalizer();
    }

    @ApiStatus.Internal public static native long _nGetFinalizer();
}