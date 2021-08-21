package org.jetbrains.jwm;

import org.jetbrains.annotations.*;
import lombok.*;

@Data
public class Screen {
    public final long _id;
    public final boolean _isPrimary;
    public final UIRect _bounds;
    /**
     * Area of the screen excluding dock/menubar
     */
    public final UIRect _workArea;
    public final float _scale;
}