package org.jetbrains.jwm;

import lombok.*;
import org.jetbrains.annotations.*;

@Data 
public class EventKeyboard implements Event {
    @ApiStatus.Internal public final Key _keyCode;
    @ApiStatus.Internal public final boolean _isPressed;
    @ApiStatus.Internal @Getter(AccessLevel.NONE) public final int _modifiers = 0;

    public EventKeyboard(Key keyCode, boolean isPressed) {
        _keyCode = keyCode;
        _isPressed = isPressed;
    }
    public EventKeyboard(int keyCode, boolean isPressed) {
        _keyCode = Key.makeFromInt(keyCode);
        _isPressed = isPressed;
    }

    public boolean isModifierDown(KeyModifier modifier) {
        return (_modifiers & modifier._mask) != 0;
    }
}