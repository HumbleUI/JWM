package org.jetbrains.jwm;

import lombok.*;
import org.jetbrains.annotations.*;

@Data 
public class EventTextInputMarked implements Event {
    @ApiStatus.Internal public final String _text;
    @ApiStatus.Internal public final int _selectionStart;
    @ApiStatus.Internal public final int _selectionEnd;
}