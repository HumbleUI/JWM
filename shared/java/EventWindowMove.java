package io.github.humbleui.jwm;

import lombok.Data;

@Data
public class EventWindowMove implements Event {
    public final int _windowLeft;
    public final int _windowTop;
}