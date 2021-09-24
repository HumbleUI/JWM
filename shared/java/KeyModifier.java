package io.github.humbleui.jwm;

import org.jetbrains.annotations.*;

// must be kept in sync with KeyModifier.hh
public enum KeyModifier {
    CAPS_LOCK    (1 << 0),
    SHIFT        (1 << 1),
    CONTROL      (1 << 2),
    ALT          (1 << 3),
    WIN_LOGO     (1 << 4),
    LINUX_META   (1 << 5),
    LINUX_SUPER  (1 << 6),
    MAC_COMMAND  (1 << 7),
    MAC_OPTION   (1 << 8),
    MAC_FN       (1 << 9);

    @ApiStatus.Internal public final int _mask;

    private KeyModifier(int mask) {
        _mask = mask;
    }
}