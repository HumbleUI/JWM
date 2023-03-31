package io.github.humbleui.jwm;

import lombok.*;
import org.jetbrains.annotations.*;

@Data 
public class EventTextInputMarked implements Event {
    /** Marked text to insert */
    @ApiStatus.Internal public final String _text;

    @ApiStatus.Internal public final int _selectionStart;

    @ApiStatus.Internal public final int _selectionEnd;

    /** Replacement region start */
    @ApiStatus.Internal public final int _replacementStart;

    /** Replacement region end */
    @ApiStatus.Internal public final int _replacementEnd;
}