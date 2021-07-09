package org.jetbrains.jwm;

import org.jetbrains.annotations.*;
import java.util.*;

public class ScreenX11 implements IScreen {
    @ApiStatus.Internal
    public int _width;
    
    @ApiStatus.Internal
    public int _height;

    @ApiStatus.Internal
    public ScreenX11() {
    }

    public int getWidth() {
        return _width;
    }
    public int getHeight() {
        return _height;
    }
    public native float getScale();

}
