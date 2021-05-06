package org.jetbrains.jwm.macos;

import java.util.function.*;
import lombok.*;
import org.jetbrains.annotations.*;
import org.jetbrains.jwm.*;
import org.jetbrains.jwm.impl.*;

public class WindowMac extends Window {
    public ContextMetal setContextMetal(boolean vsync) {
        var context = new ContextMetal(vsync);
        attach(context);
        return context;
    }
}