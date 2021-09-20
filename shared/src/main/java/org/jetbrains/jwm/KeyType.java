package org.jetbrains.jwm;

import org.jetbrains.annotations.*;

@ApiStatus.Internal
public class KeyType {
    public static final int FUNCTION   = 1 << 0;
    public static final int NAVIGATION = 1 << 1;
    public static final int ARROW      = 1 << 2;
    public static final int MODIFIER   = 1 << 3;
    public static final int LETTER     = 1 << 4;
    public static final int DIGIT      = 1 << 5;
    public static final int WHITESPACE = 1 << 6;
    public static final int MEDIA      = 1 << 7;
}