package io.github.humbleui.jwm;

import org.jetbrains.annotations.*;
import io.github.humbleui.jwm.impl.*;

public class Theme {
    /**
     * <p>Check whether OS currently uses high contrast mode.</p>
     *
     * @return  bool on Windows macOS, false otherwise
     */
    public static boolean isHighContrast() {
        assert App._onUIThread();
        if (Platform.CURRENT != Platform.WINDOWS && Platform.CURRENT != Platform.MACOS)
            return false;
        return _nIsHighContrast();
    }

    public static boolean isDark() {
        assert App._onUIThread();
        if (Platform.CURRENT != Platform.WINDOWS && Platform.CURRENT != Platform.MACOS)
            return false;
        return _nIsDark();
    }

    public static boolean isInverted() {
        assert App._onUIThread();
        if (Platform.CURRENT != Platform.MACOS)
            return false;
        return _nIsInverted();
    }

    @ApiStatus.Internal public static native boolean _nIsHighContrast();
    @ApiStatus.Internal public static native boolean _nIsDark();
    @ApiStatus.Internal public static native boolean _nIsInverted();
}
