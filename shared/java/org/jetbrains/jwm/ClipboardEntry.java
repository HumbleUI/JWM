package org.jetbrains.jwm;

import org.jetbrains.annotations.*;
import lombok.*;

@Data
public class ClipboardEntry {
    public final String _formatId;
    public final byte[] _data;
}