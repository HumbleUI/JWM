package org.jetbrains.jwm;

import lombok.*;
import java.util.*;
import org.jetbrains.annotations.*;

public enum ClipboardFormat {
    TEXT("org.jetbrains.jwm.clipboard.format.text"),
    RICH_TEXT("org.jetbrains.jwm.clipboard.format.rich_text"),
    SVG("org.jetbrains.jwm.clipboard.format.svg"),
    PNG("org.jetbrains.jwm.clipboard.format.png");
    // and etc.

    @ApiStatus.Internal @Getter public final String _formatId;

    private ClipboardFormat(String formatId) {
        _formatId = formatId;
    }
}