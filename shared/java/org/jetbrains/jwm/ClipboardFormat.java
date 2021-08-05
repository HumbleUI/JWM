package org.jetbrains.jwm;

import lombok.*;
import java.util.*;
import org.jetbrains.annotations.*;

@Data
public class ClipboardFormat {
    public static ClipboardFormat TEXT = Clipboard.registerFormatInternal("text/plain");
    public static ClipboardFormat RTF = Clipboard.registerFormatInternal("text/rtf");
    public static ClipboardFormat HTML = Clipboard.registerFormatInternal("text/html");
    public static ClipboardFormat URL = Clipboard.registerFormatInternal("text/url");

    public static ClipboardFormat WIN_BITMAP = Clipboard.registerFormatInternal("win/bitmap");
    public static ClipboardFormat WIN_TIFF = Clipboard.registerFormatInternal("win/tiff");

    public static ClipboardFormat MAC_PNG = Clipboard.registerFormatInternal("mac/png");
    public static ClipboardFormat MAC_PDF = Clipboard.registerFormatInternal("mac/pdf");
    public static ClipboardFormat MAC_COLOR = Clipboard.registerFormatInternal("mac/color");

    @ApiStatus.Internal public final String _formatId;

    @ApiStatus.Internal
    public static boolean isStringCompatible(ClipboardFormat format) {
        return format.equals(TEXT) ||
               format.equals(RTF) ||
               format.equals(HTML) ||
               format.equals(URL);
    }
}