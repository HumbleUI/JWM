package io.github.humbleui.jwm;

import lombok.Data;

@Data
public class EventWindowScreenChange implements Event {
    public static final EventWindowScreenChange INSTANCE = new EventWindowScreenChange();
}