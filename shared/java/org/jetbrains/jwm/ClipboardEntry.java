package org.jetbrains.jwm;

import org.jetbrains.annotations.*;
import lombok.*;

@Data
public class ClipboardEntry {
    public final ClipboardFormat _format;
    public final byte[] _data;
}