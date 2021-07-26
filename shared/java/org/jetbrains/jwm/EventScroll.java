package org.jetbrains.jwm;

import lombok.*;
import org.jetbrains.annotations.*;

@Data
public class EventScroll implements Event {
    @ApiStatus.Internal public final float _deltaX;
    @ApiStatus.Internal public final float _deltaY;
    @ApiStatus.Internal @Getter(AccessLevel.NONE) public final int _modifiers;

    public boolean isModifierDown(KeyModifier modifier) {
        return (_modifiers & modifier._mask) != 0;
    }
}