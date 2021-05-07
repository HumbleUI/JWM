package org.jetbrains.jwm.examples;

import org.jetbrains.jwm.*;
import org.jetbrains.skija.*;

public interface SkijaBridge extends AutoCloseable {
    void close();
    Context getContext();
    Canvas beforePaint();
    void afterPaint();
    void resize();
}