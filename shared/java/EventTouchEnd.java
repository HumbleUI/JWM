package io.github.humbleui.jwm;

import lombok.*;
import org.jetbrains.annotations.*;

@Data
public class EventTouchEnd implements Event {
    @ApiStatus.Internal public final int _id; 
}
