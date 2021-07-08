package org.jetbrains.jwm;

import org.jetbrains.annotations.ApiStatus;
import org.jetbrains.annotations.Contract;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;
import org.jetbrains.jwm.impl.RefCounted;

import java.util.function.Consumer;

public abstract class Screen extends RefCounted {
    @ApiStatus.Internal
    public Screen(long ptr) {
        super(ptr);
    }

    public abstract int getWidth();
    public abstract int getHeight();
    public abstract float getScale();

}