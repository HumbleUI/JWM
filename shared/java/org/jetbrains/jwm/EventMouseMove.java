package org.jetbrains.jwm;

import lombok.Data;

@Data
public class EventMouseMove implements Event {
    public final int _x;
    public final int _y;
}