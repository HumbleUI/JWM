package org.jetbrains.jwm;

import org.jetbrains.annotations.*;
import java.util.*;

public class ScreenX11 implements IScreen {
    @ApiStatus.Internal
    public int _x;
    
    @ApiStatus.Internal
    public int _y;

    @ApiStatus.Internal
    public int _width;
    
    @ApiStatus.Internal
    public int _height;

    @ApiStatus.Internal
    public boolean _isPrimary;

    @ApiStatus.Internal
    public ScreenX11() {}

    @Override
    public int getX() {
        return _x;
    }

    @Override
    public int getY() {
        return _y;
    }

    @Override
    public int getWidth() {
        return _width;
    }

    @Override
    public int getHeight() {
        return _height;
    }

    @Override
    public native float getScale();

    @Override
    public boolean isPrimary() {
        return _isPrimary;
    }
}
