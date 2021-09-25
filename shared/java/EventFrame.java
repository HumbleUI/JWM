package io.github.humbleui.jwm;

import lombok.Data;

@Data
public class EventFrame implements Event {
    public static final EventFrame INSTANCE = new EventFrame();
}