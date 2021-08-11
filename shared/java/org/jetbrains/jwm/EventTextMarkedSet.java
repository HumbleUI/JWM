package org.jetbrains.jwm;

import lombok.*;
import org.jetbrains.annotations.*;

@Data 
public class EventTextMarkedSet implements Event {
    @ApiStatus.Internal public final String _text;
    @ApiStatus.Internal public final int _from;
    @ApiStatus.Internal public final int _to;
}