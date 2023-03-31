package io.github.humbleui.jwm;

import lombok.*;
import org.jetbrains.annotations.*;

@Data 
public class EventTextInput implements Event {
    /** Text to insert */
    @ApiStatus.Internal public final String _text;

    /** Replacement region start */
    @ApiStatus.Internal public final int _replacementStart;

    /** Replacement region end */
    @ApiStatus.Internal public final int _replacementEnd;
}