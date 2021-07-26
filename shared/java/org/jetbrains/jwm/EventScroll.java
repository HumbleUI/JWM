package org.jetbrains.jwm;

import lombok.*;
import org.jetbrains.annotations.*;

@Data
public class EventScroll implements Event {
    @ApiStatus.Internal public final float _dx;
    @ApiStatus.Internal public final float _dy;
    @ApiStatus.Internal @Getter(AccessLevel.NONE) public final int _modifiers;

    public boolean isModifierDown(KeyModifier modifier) {
        return (_modifiers & modifier._mask) != 0;
    }
}