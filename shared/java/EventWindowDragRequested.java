package io.github.humbleui.jwm;

import lombok.Data;

@Data
public class EventWindowDragRequested implements Event {
    public final int _windowLeft;
    public final int _windowTop;

    public boolean _dragAccepted;
}