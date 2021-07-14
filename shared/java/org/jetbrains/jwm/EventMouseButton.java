package org.jetbrains.jwm;

import lombok.*;

@Data
public class EventMouseButton implements Event {
    public final MouseButton _button;
    public final boolean _isPressed;
    @Getter(AccessLevel.NONE) public final KeyModifier[] _modifiers = new KeyModifier[0]; // TODO

    public boolean isModifierDown(KeyModifier modifier) {
        for (KeyModifier m: _modifiers)
            if (m == modifier)
                return true;
        return false;
    }
}