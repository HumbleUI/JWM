package org.jetbrains.jwm;

import org.jetbrains.annotations.*;
import java.util.*;

public class ScreensX11 extends Screens {


    @ApiStatus.Internal
    public static native int _nX11GetScreenCount();

    @Override @NotNull
    public List<Screen> _getAll() {
        List<Screen> screens = new ArrayList<>();
        int count = _nX11GetScreenCount();
        for (int i = 0; i < count; ++i) {
            screens.add(null);
        } 
        return screens;
    }
}
