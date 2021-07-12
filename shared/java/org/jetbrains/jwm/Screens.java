package org.jetbrains.jwm;

import org.jetbrains.annotations.*;
import java.util.*;

import java.util.function.Consumer;

public abstract class Screens {

    @ApiStatus.Internal
    public abstract List<IScreen> _getAll();

}