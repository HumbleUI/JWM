package org.jetbrains.jwm;

import java.util.concurrent.*;
import java.util.function.*;
import java.util.Optional;
import org.jetbrains.annotations.*;
import org.jetbrains.jwm.impl.*;

public abstract class Clipboard extends RefCounted {

    @ApiStatus.Internal
    public Clipboard(long ptr) {
        super(ptr);
    }

    void putString(String text) {
        _nPutString(text);
    }

    void putEntries(ClipboardEntry[] entries) {
        _nPutEntries(entries);
    }

    Optional<ClipboardFormat> preferredFormat(ClipboardFormat[] formats) {
        return _nPreferredFormat(formats);
    }

    Optional<String> getString() {
        return _nGetString();
    }

    Optional<byte[]> getEntry(ClipboardFormat format) {
        return _nGetEntry(format);
    }

    @Override
    public void close() {
        _nClose();
        super.close();
    }

    @ApiStatus.Internal public static native long _nMake();
    @ApiStatus.Internal public native void _nPutString(String text);
    @ApiStatus.Internal public native void _nPutEntries(ClipboardEntry[] entries);
    @ApiStatus.Internal public native Optional<String> _nGetString();
    @ApiStatus.Internal public native Optional<ClipboardFormat> _nPreferredFormat(ClipboardFormat[] formats);
    @ApiStatus.Internal public native Optional<byte[]> _nGetEntry(ClipboardFormat format);
    @ApiStatus.Internal public native void _nClose();
}