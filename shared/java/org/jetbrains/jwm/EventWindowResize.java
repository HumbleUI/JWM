package org.jetbrains.jwm;

import lombok.Data;

@Data
public class EventWindowResize implements Event {
    public final int _windowWidth;
    public final int _windowHeight;
    public final int _contentWidth;
    public final int _contentHeight;
}