package org.jetbrains.jwm;

import lombok.Data;

@Data
public class EventWindowMinimize implements Event {
    public static final EventWindowMinimize INSTANCE = new EventWindowMinimize();
}