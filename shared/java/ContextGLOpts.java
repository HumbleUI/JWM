package org.jetbrains.jwm;

import lombok.*;
import org.jetbrains.annotations.*;

@ToString @EqualsAndHashCode @Getter
public class ContextGLOpts {
    public static final ContextGLOpts DEFAULT = new ContextGLOpts(true, false);

    public final boolean _vsync;
    public final boolean _displayLink;

    @ApiStatus.Internal
    public ContextGLOpts(boolean vsync, boolean displayLink) {
        _vsync = vsync;
        _displayLink = displayLink;
    }

    public ContextGLOpts withVsync(boolean vsync) {
        return new ContextGLOpts(vsync, _displayLink);
    }

    public ContextGLOpts withDisplayLink(boolean displayLink) {
        return new ContextGLOpts(_vsync, displayLink);
    }
}