package org.jetbrains.jwm;

import lombok.*;
import java.util.*;
import org.jetbrains.annotations.*;

public class ClipboardFormat {
    public static final ClipboardFormat TEXT = new ClipboardFormat("org.jetbrains.jwm.clipboard_format.text");
    public static final ClipboardFormat RTF  = new ClipboardFormat("org.jetbrains.jwm.clipboard_format.rich_text");
    public static final ClipboardFormat SVG  = new ClipboardFormat("org.jetbrains.jwm.clipboard_format.svg");
    public static final ClipboardFormat PNG  = new ClipboardFormat("org.jetbrains.jwm.clipboard_format.png");
    // and etc.

    @ApiStatus.Internal @Getter public final String _formatId;

    @ApiStatus.Internal
    private ClipboardFormat(String formatId) {
        _formatId = formatId;
    }

    public static ClipboardFormat makeFormat(String formatId) {
        // Concept: this function will throw if is unable to register this format type
        _nMakeFormat(formatId);
        return new ClipboardFormat(formatId);
    }

    @ApiStatus.Internal public static native void _nMakeFormat(String formatId);
}