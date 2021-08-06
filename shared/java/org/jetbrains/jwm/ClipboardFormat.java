package org.jetbrains.jwm;

import lombok.*;
import java.util.*;
import org.jetbrains.annotations.*;

@Data
public class ClipboardFormat {
    public static ClipboardFormat TEXT = Clipboard._registerPredefinedFormat("text/plain");
    public static ClipboardFormat RTF = Clipboard._registerPredefinedFormat("text/rtf");
    public static ClipboardFormat HTML = Clipboard._registerPredefinedFormat("text/html");
    public static ClipboardFormat URL = Clipboard._registerPredefinedFormat("text/url");

    public static ClipboardFormat WIN_BITMAP = Clipboard._registerPredefinedFormat("win/bitmap");
    public static ClipboardFormat WIN_TIFF = Clipboard._registerPredefinedFormat("win/tiff");

    public static ClipboardFormat MAC_PNG = Clipboard._registerPredefinedFormat("mac/png");
    public static ClipboardFormat MAC_PDF = Clipboard._registerPredefinedFormat("mac/pdf");
    public static ClipboardFormat MAC_COLOR = Clipboard._registerPredefinedFormat("mac/color");

    @ApiStatus.Internal public final String _formatId;
}