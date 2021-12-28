package io.github.humbleui.jwm;

import lombok.*;
import io.github.humbleui.types.*;

@AllArgsConstructor @Data
public class EventWindowResize implements Event {
    public final int _windowWidth;
    public final int _windowHeight;
    public final int _contentWidth;
    public final int _contentHeight;

    public EventWindowResize(Window w) {
        IRect wr = w.getWindowRect();
        _windowWidth = wr.getWidth();
        _windowHeight = wr.getHeight();
        IRect cr = w.getContentRect();
        _contentWidth = cr.getWidth();
        _contentHeight = cr.getHeight();
    }
}