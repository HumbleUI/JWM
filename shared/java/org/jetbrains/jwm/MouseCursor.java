package org.jetbrains.jwm;

import org.jetbrains.annotations.*;

public enum MouseCursor {
    ARROW        ,
    CROSSHAIR    ,
    HELP         ,
    POINTING_HAND,
    IBEAM        ,
    UPARROW      ,
    NOT_ALLOWED  ,
    WAIT         ;

    @ApiStatus.Internal public static final MouseCursor[] _values = values();

    public static MouseCursor makeFromInt(int v) {
        return _values[v];
    }
}