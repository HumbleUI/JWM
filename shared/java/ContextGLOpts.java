package org.jetbrains.jwm;

import lombok.*;
import org.jetbrains.annotations.*;

@ToString @EqualsAndHashCode @Getter
public class ContextGLOpts {
    public static final ContextGLOpts DEFAULT = new ContextGLOpts();

    @ApiStatus.Internal
    public ContextGLOpts() {
    }
}