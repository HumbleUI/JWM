package io.github.humbleui.jwm;

import lombok.Data;

@Data
public class EventWindowCloseRequest implements Event {
    public static final EventWindowCloseRequest INSTANCE = new EventWindowCloseRequest();
}