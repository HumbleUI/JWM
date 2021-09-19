package org.jetbrains.jwm.examples;

import java.util.function.*;
import org.jetbrains.jwm.*;
import org.jetbrains.skija.*;

public abstract class Panel implements Consumer<Event> {
    public final Window window;
    public int lastWidth = 0, lastHeight = 0, lastX = 0, lastY = 0;
    public float lastScale = 1f;
    public boolean drawBG = true;

    public Panel(Window window) {
        this.window = window;
    } 

    public abstract void paintImpl(Canvas canvas, int width, int height, float scale);

    @Override
    public void accept(Event e) {}

    public void paint(Canvas canvas, int x, int y, int width, int height, float scale) {
        int count = canvas.save();
        canvas.translate(x, y);
        canvas.clipRect(Rect.makeXYWH(0, 0, width, height));
        if (drawBG) {
            try (var paint = new Paint()) {
                paint.setColor(0x20000000);
                canvas.drawRRect(RRect.makeXYWH(0, 0, width, height, 4 * scale), paint);
            }
        }
        paintImpl(canvas, width, height, scale);
        canvas.restoreToCount(count);

        lastWidth = width;
        lastHeight = height;
        lastX = x;
        lastY = y;
        lastScale = scale;
    }

    public boolean contains(int x, int y) {
        return UIRect.makeXYWH(lastX, lastY, lastWidth, lastHeight).contains(x, y);
    }

    public String capitalize(String s) {
        return s.substring(0, 1).toUpperCase() + s.substring(1).toLowerCase();
    }
}