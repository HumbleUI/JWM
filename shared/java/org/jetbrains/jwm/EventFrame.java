package org.jetbrains.jwm;

import lombok.*;

@Data
public class EventFrame implements Event {
    public static final EventFrame INSTANCE = new EventFrame();
}