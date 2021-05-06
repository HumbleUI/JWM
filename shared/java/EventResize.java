package org.jetbrains.jwm;

import lombok.*;

@Data
public class EventResize implements Event {
    public final int _width;
    public final int _height;
}