package io.github.humbleui.jwm;

import lombok.*;
import org.jetbrains.annotations.*;

@Data 
public class EventTextInput implements Event {
    @ApiStatus.Internal public final String _text;
    // @ApiStatus.Internal public final int _from;
    // @ApiStatus.Internal public final int _to;
}