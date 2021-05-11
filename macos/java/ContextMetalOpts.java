package org.jetbrains.jwm;

import lombok.*;
import org.jetbrains.annotations.*;

@ToString @EqualsAndHashCode @Getter
public class ContextMetalOpts {
    public static final ContextMetalOpts DEFAULT = new ContextMetalOpts(true, false, false);

    public final boolean _vsync;
    public final boolean _transact;
    public final boolean _displayLink;

    @ApiStatus.Internal
    public ContextMetalOpts(boolean vsync, boolean transact, boolean displayLink) {
        _vsync = vsync;
        _transact = transact;
        _displayLink = displayLink;
    }

    public ContextMetalOpts withVsync(boolean vsync) {
        return new ContextMetalOpts(vsync, _transact, _displayLink);
    }

    public ContextMetalOpts withTransact(boolean transact) {
        return new ContextMetalOpts(_vsync, transact, _displayLink);
    }

    public ContextMetalOpts withDisplayLink(boolean displayLink) {
        return new ContextMetalOpts(_vsync, _transact, displayLink);
    }
}