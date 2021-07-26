package org.jetbrains.jwm;

import lombok.Data;

@Data
public class EventWindowMove implements Event {
    public final int _left;
    public final int _top;
}