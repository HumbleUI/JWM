package io.github.humbleui.jwm;

import lombok.Data;

@Data
public class EventWindowFocusOff implements Event {
    public static final EventWindowFocusOff INSTANCE = new EventWindowFocusOff();
}
