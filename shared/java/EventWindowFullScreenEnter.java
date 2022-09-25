package io.github.humbleui.jwm;

import lombok.Data;

@Data
public class EventWindowFullScreenEnter implements Event {
    public static final EventWindowFullScreenEnter INSTANCE = new EventWindowFullScreenEnter();
}