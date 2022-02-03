package io.github.humbleui.jwm;

import lombok.Data;

@Data
public class EventWindowFocusOut implements Event {
    public static final EventWindowFocusOut INSTANCE = new EventWindowFocusOut();
}