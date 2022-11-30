package io.github.humbleui.jwm;

import lombok.*;
import org.jetbrains.annotations.*;

@Data
public class EventTrackpadTouchStart implements Event {
    @ApiStatus.Internal public final int _id;
    @ApiStatus.Internal public final float _fracX;
    @ApiStatus.Internal public final float _fracY;
    @ApiStatus.Internal public final int _deviceId;
    @ApiStatus.Internal public final float _deviceWidth;
    @ApiStatus.Internal public final float _deviceHeight;
}
