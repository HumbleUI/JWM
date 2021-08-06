package org.jetbrains.jwm;

import org.jetbrains.annotations.*;
import lombok.*;

@Data
public class ClipboardEntry {
    @ApiStatus.Internal public final ClipboardFormat _format;
    @ApiStatus.Internal public final byte[] _data;

    @NotNull
    public static ClipboardEntry make(ClipboardFormat format, byte[] data) {
        return new ClipboardEntry(format, data);
    }

    @NotNull @SneakyThrows
    public static ClipboardEntry makePlainText(String text) {
        return makeString(ClipboardFormat.TEXT, text);
    }

    @NotNull @SneakyThrows
    public static ClipboardEntry makeHTML(String text) {
        return makeString(ClipboardFormat.HTML, text);
    }

    @NotNull @SneakyThrows
    public static ClipboardEntry makeRTF(String text) {
        return makeString(ClipboardFormat.RTF, text);
    }

    @NotNull @SneakyThrows
    public static ClipboardEntry makeString(ClipboardFormat format, String text) {
        return make(format, text.getBytes("UTF-16LE"));
    }

    @NotNull @SneakyThrows
    public String getString() {
        return new String(_data, "UTF-16LE");
    }
}