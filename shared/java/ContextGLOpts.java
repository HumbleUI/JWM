package org.jetbrains.jwm;

import lombok.*;
import org.jetbrains.annotations.*;

@ToString @EqualsAndHashCode @Getter
public class ContextGLOpts {
    public static final ContextGLOpts DEFAULT = new ContextGLOpts(true);

    public final boolean _vsync;

    @ApiStatus.Internal
    public ContextGLOpts(boolean vsync) {
        _vsync = vsync;
    }

    public ContextGLOpts withVsync(boolean vsync) {
        return new ContextGLOpts(vsync);
    }
}