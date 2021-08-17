package org.jetbrains.jwm;

import lombok.Data;

@Data
public class EventEnvironmentChange implements Event {
    public static final EventEnvironmentChange INSTANCE = new EventEnvironmentChange();
}