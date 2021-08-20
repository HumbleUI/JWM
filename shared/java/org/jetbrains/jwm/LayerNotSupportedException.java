package org.jetbrains.jwm;

import java.lang.*;
import org.jetbrains.annotations.*;

class LayerNotSupportedException extends RuntimeException {
    @ApiStatus.Internal
    public LayerNotSupportedException(String message) {
        super(message);
    }
}