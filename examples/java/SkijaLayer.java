package io.github.humbleui.jwm.examples;

import io.github.humbleui.jwm.Layer;
import org.jetbrains.skija.Canvas;

public interface SkijaLayer extends Layer {
    Canvas beforePaint();
    void afterPaint();
}