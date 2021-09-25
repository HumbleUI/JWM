package io.github.humbleui.jwm;

import org.jetbrains.annotations.*;
import io.github.humbleui.jwm.impl.*;

public class Theme {

    /**
     * <p>>Check whether OS currently uses high contrast mode.</p>
     *
     * @return          bool;
     */
    public static boolean isHighContrast() {
        assert _onUIThread();
        return _nIsHighContrast();
    }


    @ApiStatus.Internal public static boolean _onUIThread() {
        return App._onUIThread();
    }

    @ApiStatus.Internal public static native boolean _nIsHighContrast();
}
