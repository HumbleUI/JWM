package org.jetbrains.jwm;

import org.jetbrains.annotations.ApiStatus;
import org.jetbrains.jwm.impl.RefCounted;

public abstract class Screen extends RefCounted {
    @ApiStatus.Internal
    public Screen(long ptr) {
        super(ptr);
    }

    public abstract int getX();

    public abstract int getY();

    public abstract int getWidth();

    public abstract int getHeight();

    public abstract float getScale();

    public abstract boolean isPrimary();

    @Override
    public void close() {
        super.close();
    }
}