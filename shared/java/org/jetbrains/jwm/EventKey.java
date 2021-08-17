
package org.jetbrains.jwm;

import lombok.*;
import org.jetbrains.annotations.*;

@Data 
public class EventKey implements Event {
    @ApiStatus.Internal public final Key _key;
    @ApiStatus.Internal public final boolean _isPressed;
    @ApiStatus.Internal @Getter(AccessLevel.NONE) public final int _modifiers;

    public EventKey(int keyCode, boolean isPressed, int modifiers) {
        _key = Key.makeFromInt(keyCode);
        _isPressed = isPressed;
        _modifiers = modifiers;
    }

    public boolean isModifierDown(KeyModifier modifier) {
        return (_modifiers & modifier._mask) != 0;
    }
}