package org.jetbrains.jwm;

import org.jetbrains.annotations.ApiStatus;
import org.jetbrains.annotations.Contract;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

import java.util.function.Consumer;

public interface IScreen {

    public abstract int getX();
    public abstract int getY();

    public abstract int getWidth();
    public abstract int getHeight();

    public abstract float getScale();

    public abstract boolean isPrimary();
}