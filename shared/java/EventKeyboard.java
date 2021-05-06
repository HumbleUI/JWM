package org.jetbrains.jwm;

import lombok.*;

@Data
public class EventKeyboard implements Event {
    public final int _keyCode;
    public final boolean _isPressed;
}