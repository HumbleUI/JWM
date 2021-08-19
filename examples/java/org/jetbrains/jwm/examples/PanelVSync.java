package org.jetbrains.jwm.examples;

import java.util.function.*;
import org.jetbrains.jwm.*;
import org.jetbrains.skija.*;

public class PanelVSync extends Panel {
    public boolean frame = false;

    @Override
    public void paintImpl(Canvas canvas, int width, int height, float scale) {
        try (var paint = new Paint();
             var line = TextLine.make("VSYNC", Example.FONT48))
        {
            paint.setColor(0xFFE0E0E0);
            canvas.drawRRect(RRect.makeXYWH(0, 0, width, height, 4 * scale), paint);
            
            paint.setColor(frame ? 0xFFEF8784 : 0xFFA1FCFE);
            FontMetrics metrics = Example.FONT48.getMetrics();
            var baseline = height / 2 + metrics.getCapHeight() / 2;
            canvas.drawTextLine(line, (width - line.getWidth()) / 2, baseline, paint);
        }

        frame = !frame;
    }
}
