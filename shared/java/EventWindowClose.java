package io.github.humbleui.jwm;

import lombok.Data;

@Data
public class EventWindowClose implements Event {
    public static final EventWindowClose INSTANCE = new EventWindowClose();
}