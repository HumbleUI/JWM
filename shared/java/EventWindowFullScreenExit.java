package io.github.humbleui.jwm;

import lombok.Data;

@Data
public class EventWindowFullScreenExit implements Event {
    public static final EventWindowFullScreenExit INSTANCE = new EventWindowFullScreenExit();
}