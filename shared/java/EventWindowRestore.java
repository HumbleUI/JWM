package io.github.humbleui.jwm;

import lombok.Data;

@Data
public class EventWindowRestore implements Event {
    public static final EventWindowRestore INSTANCE = new EventWindowRestore();
}