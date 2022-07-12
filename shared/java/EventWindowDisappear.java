package io.github.humbleui.jwm;

import lombok.Data;

@Data
public class EventWindowDisappear implements Event {
    public static final EventWindowDisappear INSTANCE = new EventWindowDisappear();
}
