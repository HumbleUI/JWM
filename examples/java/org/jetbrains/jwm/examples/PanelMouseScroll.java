package org.jetbrains.jwm.examples;

import java.util.*;
import java.util.function.*;
import java.util.stream.*;

import org.jetbrains.jwm.*;
import org.jetbrains.skija.*;

public class PanelMouseScroll extends Panel {
    public Point scroll = new Point(0, 0);

    @Override
    public void accept(Event e) {
        if (e instanceof EventMouseScroll ee) {
            scroll = scroll.offset(ee.getDeltaX() * lastScale, ee.getDeltaY() * lastScale);
        }
    }

    @Override
    public void paintImpl(Canvas canvas, int width, int height, float scale) {
        int halfWidth = width / 2;
        int halfHeight = height / 2;
        int step = (int) (25 * scale);
        int halfStep = step / 2;

        try (var paint = new Paint().setMode(PaintMode.STROKE).setStrokeWidth(2 * scale).setColor(0x40FFFFFF)) {
            for (int x = (int) Math.ceil(-scroll.getX() / step - 1) * step; x + scroll.getX() < width; x += step) {
                canvas.drawLine(scroll.getX() + x, halfHeight - 2 * scale, scroll.getX() + x, halfHeight + 2 * scale, paint);
                canvas.drawLine(scroll.getX() + x + halfStep, halfHeight - 4 * scale, scroll.getX() + x + halfStep, halfHeight + 4 * scale, paint);
            }

            for (int y = (int) Math.ceil(-scroll.getY() / step - 1) * step; y + scroll.getY() < height; y += step) {
                canvas.drawLine(halfWidth - 2 * scale, scroll.getY() + y, halfWidth + 2 * scale, scroll.getY() + y, paint);
                canvas.drawLine(halfWidth - 4 * scale, scroll.getY() + y + halfStep, halfWidth + 4 * scale, scroll.getY() + y + halfStep, paint);
            }
        }
    }
}
