package org.jetbrains.jwm;

import lombok.*;

@Data
public class EventReconfigure implements Event {
    public static final EventReconfigure INSTANCE = new EventReconfigure();
}