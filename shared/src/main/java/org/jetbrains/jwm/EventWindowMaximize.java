package org.jetbrains.jwm;

import lombok.Data;

@Data
public class EventWindowMaximize implements Event {
    public static final EventWindowMaximize INSTANCE = new EventWindowMaximize();
}