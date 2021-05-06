package org.jetbrains.jwm;

import lombok.*;

@Data
public class EventPaint implements Event {
    public static final EventPaint INSTANCE = new EventPaint();
}