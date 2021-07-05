package org.jetbrains.jwm.examples;

import org.jetbrains.jwm.Layer;
import org.jetbrains.skija.Canvas;

public interface SkijaLayer extends Layer {
    Canvas beforePaint();
    void afterPaint();
}