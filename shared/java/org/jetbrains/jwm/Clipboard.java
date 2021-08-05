package org.jetbrains.jwm;

import lombok.*;
import java.util.*;
import java.util.function.*;
import java.util.concurrent.*;
import org.jetbrains.annotations.*;
import org.jetbrains.jwm.impl.*;

public class Clipboard {
    @ApiStatus.Internal
    public static Map<String, ClipboardFormat> _formats = Collections.synchronizedMap(new HashMap<String, ClipboardFormat>());

    public static void set(ClipboardEntry ... entries) {
        assert entries.length > 0;
        _nSet(entries);
    }

    @Nullable
    public static ClipboardEntry get(ClipboardFormat ... formats) {
        assert formats.length > 0;
        return _nGet(formats);
    }

    public static ClipboardFormat[] getFormats() {
        return _nGetFormats();
    }

    public static void clear() {
        _nClear();
    }

    @NotNull @SneakyThrows
    public static ClipboardFormat registerFormat(String formatId) {
        ClipboardFormat format = _formats.get(formatId);

        if (format != null)
            return format;

        if (!_nRegisterFormat(formatId))
            throw new RuntimeException("Failed to register format: " + formatId);

        return registerFormatInternal(formatId);
    }

    @ApiStatus.Internal @NotNull
    public static ClipboardFormat registerFormatInternal(String formatId) {
        ClipboardFormat format = new ClipboardFormat(formatId);
        _formats.put(formatId, format);
        return format;
    }

    @ApiStatus.Internal public static native void _nSet(ClipboardEntry[] entries);
    @ApiStatus.Internal public static native ClipboardEntry _nGet(ClipboardFormat[] formats);
    @ApiStatus.Internal public static native ClipboardFormat[] _nGetFormats();
    @ApiStatus.Internal public static native void _nClear();
    @ApiStatus.Internal public static native boolean _nRegisterFormat(String formatId);
}