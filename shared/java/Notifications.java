package io.github.humbleui.jwm;

import org.jetbrains.annotations.*;
import io.github.humbleui.jwm.impl.*;

public class Notifications {
    public static void notify(String title, String body) {
        assert App._onUIThread();
        throw new UnsupportedOperationException("Not implemented yet");
    }

    @ApiStatus.Internal public static native void _nNotify(String title, String body);
}
