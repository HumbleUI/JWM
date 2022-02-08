package io.github.humbleui.jwm;

import lombok.Data;

@Data
public class EventWindowAppear implements Event {
    public static final EventWindowAppear INSTANCE = new EventWindowAppear();
}
