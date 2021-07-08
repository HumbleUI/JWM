package org.jetbrains.jwm;

import org.jetbrains.annotations.ApiStatus;
import org.jetbrains.annotations.Contract;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;
import org.jetbrains.jwm.impl.RefCounted;
import java.util.List;
import java.util.ArrayList;

import java.util.function.Consumer;

public abstract class Screens {
    @ApiStatus.Internal
    public static Screens _instance = _nInstantiate();

    @ApiStatus.Internal
    public static native Screens _nInstantiate();

    public static List<Screen> getAll() {
        return null;
    }
}