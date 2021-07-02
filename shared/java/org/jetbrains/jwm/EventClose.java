package org.jetbrains.jwm;

import lombok.*;

@Data
public class EventClose implements Event {
    public static final EventClose INSTANCE = new EventClose();
}