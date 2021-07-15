package org.jetbrains.jwm;

import org.jetbrains.annotations.*;

public enum KeyModifier {
    SHIFT(1 << 0),
    CTRL(1 << 1),
    ALT(1 << 2),
    CMD(1 << 3),
    WINDOWS(1 << 4),
    META(1 << 5);

    @ApiStatus.Internal public final int _mask;

    private KeyModifier(int mask) {
        _mask = mask;
    }
}