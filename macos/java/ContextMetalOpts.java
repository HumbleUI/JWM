package org.jetbrains.jwm;

import lombok.*;
import org.jetbrains.annotations.*;

@ToString @EqualsAndHashCode @Getter
public class ContextMetalOpts {
    public static final ContextMetalOpts DEFAULT = new ContextMetalOpts(true, false);

    public final boolean _vsync;
    public final boolean _transact;

    @ApiStatus.Internal
    public ContextMetalOpts(boolean vsync, boolean transact) {
        _vsync = vsync;
        _transact = transact;
    }

    public ContextMetalOpts withVsync(boolean vsync) {
        return new ContextMetalOpts(vsync, _transact);
    }

    public ContextMetalOpts withTransact(boolean transact) {
        return new ContextMetalOpts(_vsync, transact);
    }
}