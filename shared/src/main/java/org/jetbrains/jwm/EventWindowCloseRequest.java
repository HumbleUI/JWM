package org.jetbrains.jwm;

import lombok.Data;

@Data
public class EventWindowCloseRequest implements Event {
    public static final EventWindowCloseRequest INSTANCE = new EventWindowCloseRequest();
}