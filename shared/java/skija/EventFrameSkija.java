package io.github.humbleui.jwm;

import io.github.humbleui.skija.*;
import lombok.*;
import org.jetbrains.annotations.*;

@lombok.Data
public class EventFrameSkija implements Event {
    @ApiStatus.Internal public final Surface _surface;

    @Override
    public String toString() {
        return "EventFrameSkija(_surface=" + System.identityHashCode(_surface) + ")";
    }
}