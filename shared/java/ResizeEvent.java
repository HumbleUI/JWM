package org.jetbrains.jwm;

import lombok.*;

@Data
public class ResizeEvent implements Event {
    public final int _width;
    public final int _height;
}