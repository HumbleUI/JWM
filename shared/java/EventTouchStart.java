package io.github.humbleui.jwm;

import lombok.*;
import org.jetbrains.annotations.*;

@Data
public class EventTouchStart implements Event {
    @ApiStatus.Internal public final int _id;
    @ApiStatus.Internal public final float _fracX;
    @ApiStatus.Internal public final float _fracY;
    @ApiStatus.Internal public final int _deviceId;
    @ApiStatus.Internal public final float _deviceWidth;
    @ApiStatus.Internal public final float _deviceHeight;
    @ApiStatus.Internal public final TouchType _touchType;

    public EventTouchStart(int id, float fracX, float fracY, int deviceId, float deviceWidth, float deviceHeight, int touchType) {
        _id = id;
        _fracX = fracX;
        _fracY = fracY;
        _deviceId = deviceId;
        _deviceWidth = deviceWidth;
        _deviceHeight = deviceHeight;
        _touchType = TouchType.makeFromInt(touchType);
    }
}
