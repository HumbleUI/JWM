package org.jetbrains.jwm;

import lombok.*;
import java.util.*;
import org.jetbrains.annotations.*;

@Data
public class ClipboardFormat {
    /**
     * <p>Represents a plain string.</p>
     * <p>Uses system built-in format and encoding for proper text transfer.</p>
     */
    public static ClipboardFormat TEXT = Clipboard._registerPredefinedFormat("text/plain");

    /**
     * <p>Represents a rich text formatted string.</p>
     * <p>Uses system built-in format (if present) and encoding for proper text transfer.</p>
     */
    public static ClipboardFormat RTF = Clipboard._registerPredefinedFormat("text/rtf");

    /**
     * <p>Represents an HTML formatted string.</p>
     * <p>Uses system built-in format (if present) and encoding for proper text transfer.</p>
     */
    public static ClipboardFormat HTML = Clipboard._registerPredefinedFormat("text/html");

    /**
     * <p>Represents URL, encoded as string.</p>
     * <p>Uses system built-in format (if present) and encoding for proper text transfer.</p>
     */
    public static ClipboardFormat URL = Clipboard._registerPredefinedFormat("text/url");

    public static ClipboardFormat WIN_BITMAP = Clipboard._registerPredefinedFormat("win/bitmap");
    public static ClipboardFormat WIN_TIFF = Clipboard._registerPredefinedFormat("win/tiff");

    public static ClipboardFormat MAC_PNG = Clipboard._registerPredefinedFormat("mac/png");
    public static ClipboardFormat MAC_PDF = Clipboard._registerPredefinedFormat("mac/pdf");
    public static ClipboardFormat MAC_COLOR = Clipboard._registerPredefinedFormat("mac/color");

    @ApiStatus.Internal public final String _formatId;
}