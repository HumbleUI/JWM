package org.jetbrains.jwm;

import org.jetbrains.annotations.*;
import lombok.*;

@Data
public class Screen {
    public final long _id;
    public final int _x;
    public final int _y;
    public final int _width;
    public final int _height;
    public final float _scale;
    public final boolean _isPrimary;
}