package io.github.humbleui.jwm;

import org.jetbrains.annotations.*;

public enum ZOrder {
    /** Default z-order */
    NORMAL,

    /** Default “Always on top” behavior */
    FLOATING,

    /** Windows/macOS only */
    MODAL_PANEL,

    /** Windows/macOS only */
    MAIN_MENU,

    /** Windows/macOS only */
    STATUS,

    /** Windows/macOS only */
    POP_UP_MENU,

    /** Windows/macOS only */
    SCREEN_SAVER;

    @ApiStatus.Internal public static final ZOrder[] _values = values();
}