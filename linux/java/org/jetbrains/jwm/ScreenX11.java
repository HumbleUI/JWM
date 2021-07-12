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
    public native float getScale();

}
