package org.jetbrains.jwm;

import lombok.*;
import org.jetbrains.annotations.*;

@ToString @EqualsAndHashCode @Getter
public class ContextMetalOpts {
    public static final ContextMetalOpts DEFAULT = new ContextMetalOpts();

    @ApiStatus.Internal
    public ContextMetalOpts() {
    }
}