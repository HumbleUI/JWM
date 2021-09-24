package io.github.humbleui.jwm;

import lombok.*;
import org.jetbrains.annotations.*;

@Data
public class EventMouseButton implements Event {
    @ApiStatus.Internal public final MouseButton _button;
    @ApiStatus.Internal public final boolean _isPressed;
    @ApiStatus.Internal public final int _modifiers;

    public EventMouseButton(int button, boolean isPressed, int modifiers) {
        _button = MouseButton.makeFromInt(button);
        _isPressed = isPressed;
        _modifiers = modifiers;
    }

    public boolean isModifierDown(KeyModifier modifier) {
        return (_modifiers & modifier._mask) != 0;
    }
}