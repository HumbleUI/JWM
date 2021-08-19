package org.jetbrains.jwm.examples;

import org.jetbrains.jwm.*;
import org.jetbrains.skija.*;

public class PanelAnimation extends Panel {
    public int angle = 0;

    @Override
    public void paintImpl(Canvas canvas, int width, int height, float scale) {
        var radius = Math.min(width / 2, height / 2);
        
        try (var paint = new Paint()) {
            canvas.save();
            canvas.translate(width / 2, height / 2);
            paint.setColor(0xFFFFFFFF);
            canvas.drawCircle(0, 0, radius, paint);
            canvas.rotate(angle);
            paint.setColor(0xFF264653);
            canvas.drawRect(Rect.makeXYWH(-7, -radius, 14, radius * 2), paint);
            canvas.restore();
        }

        angle = (angle + 3) % 360;
    }
}