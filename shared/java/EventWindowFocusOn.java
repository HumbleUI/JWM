package io.github.humbleui.jwm;

import lombok.Data;

@Data
public class EventWindowFocusOn implements Event {
    public static final EventWindowFocusOn INSTANCE = new EventWindowFocusOn();
}
