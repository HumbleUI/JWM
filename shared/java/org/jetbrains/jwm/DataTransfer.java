package org.jetbrains.jwm;

import org.jetbrains.annotations.*;
import java.util.*;
import java.nio.charset.StandardCharsets;

public class DataTransfer {
    @ApiStatus.Internal
    public HashMap<String, byte[]> _data = new HashMap<String, byte[]>();

    public DataTransfer() {

    }

    public byte[] get(String mimeType) {
        return _data.get(mimeType);
    }

    public String getString() {
        byte[] b = get("text/plain");
        if (b != null) {
            return new String(b, StandardCharsets.UTF_8);
        }
        return null;
    }

    public void put(String mimeType, byte[] data) {
        _data.put(mimeType, data);
    }
}