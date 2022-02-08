package io.github.humbleui.jwm;

import lombok.Data;

@Data
public class EventWindowFocusIn implements Event {
    public static final EventWindowFocusIn INSTANCE = new EventWindowFocusIn();
}