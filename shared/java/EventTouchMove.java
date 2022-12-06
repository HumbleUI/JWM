package io.github.humbleui.jwm;

import lombok.*;
import org.jetbrains.annotations.*;

@Data
public class EventTouchMove implements Event {
    @ApiStatus.Internal public final int _id; 
    @ApiStatus.Internal public final float _fracX;
    @ApiStatus.Internal public final float _fracY;
}
