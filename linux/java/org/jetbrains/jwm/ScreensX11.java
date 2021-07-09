package org.jetbrains.jwm;

import org.jetbrains.annotations.*;
import java.util.*;

public class ScreensX11 extends Screens {


    @ApiStatus.Internal
    public static native IScreen[] _nX11GetAll();

    @Override @NotNull
    public List<IScreen> _getAll() {
        return Arrays.asList(_nX11GetAll());
    }
}
