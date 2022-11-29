package io.github.humbleui.jwm;

import lombok.*;
import org.jetbrains.annotations.*;

@Data
public class EventMouseMove implements Event {
    @ApiStatus.Internal public final int _x;
    @ApiStatus.Internal public final int _y;
    @ApiStatus.Internal @Getter(AccessLevel.NONE) public final int _buttons;
    @ApiStatus.Internal @Getter(AccessLevel.NONE) public final int _modifiers;

    public boolean isButtonDown(MouseButton button) {
        return (_buttons & button._mask) != 0;
    }

    public boolean isModifierDown(KeyModifier modifier) {
        return (_modifiers & modifier._mask) != 0;
    }
}
