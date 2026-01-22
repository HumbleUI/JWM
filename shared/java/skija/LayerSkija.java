package io.github.humbleui.jwm;

import io.github.humbleui.skija.*;
import org.jetbrains.annotations.*;

public interface LayerSkija {
    @NotNull
    static ColorSpace windowColorSpace(Window window) {
        Screen screen = window.getScreen();
        if (screen != null) {
            byte[] iccProfile = screen.getICCProfile();
            if (iccProfile != null) {
                ColorSpace cs = ColorSpace.makeFromICCProfile(iccProfile);
                if (cs != null) {
                    return cs;
                }
            }
        }

        return ColorSpace.getSRGB();
    }
}
