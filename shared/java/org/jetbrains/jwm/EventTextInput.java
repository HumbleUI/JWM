package org.jetbrains.jwm;

import lombok.*;
import org.jetbrains.annotations.*;

@Data 
public class EventTextInput implements Event {
    @ApiStatus.Internal public final String _text;

    public EventTextInput(String text) {
        _text = text;
    }
}