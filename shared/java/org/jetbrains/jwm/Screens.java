package org.jetbrains.jwm;

import org.jetbrains.annotations.*;
import java.util.*;

import java.util.function.Consumer;

public abstract class Screens {
    @ApiStatus.Internal
    public static Screens _instance = _nInstantiate();

    @ApiStatus.Internal
    public static native Screens _nInstantiate();


    @ApiStatus.Internal
    public abstract List<Screen> _getAll();


    public static List<Screen> getAll() {
        return _instance._getAll();
    }
}