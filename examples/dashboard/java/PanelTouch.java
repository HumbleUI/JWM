package io.github.humbleui.jwm.examples;

import java.util.*;
import java.util.function.*;
import java.util.stream.*;

import io.github.humbleui.jwm.*;
import io.github.humbleui.skija.*;
import io.github.humbleui.types.*;

public class PanelTouch extends Panel {
    public Map<Integer, Point> touches = Collections.synchronizedMap(new HashMap<Integer, Point>()); // coords
    public Map<Integer, Point> devices = Collections.synchronizedMap(new TreeMap<Integer, Point>()); // sizes
    public Map<Integer, Integer> touchDevices = Collections.synchronizedMap(new HashMap<Integer, Integer>()); // touch -> device

    public final float radius = 32;

    public PanelTouch(Window window) {
        super(window);
    }

    @Override
    public void accept(Event e) {
        if (e instanceof EventTouchStart ee) {
            touchDevices.put(ee.getDeviceId(), ee.getId());
            devices.put(ee.getDeviceId(), new Point(ee.getDeviceWidth(), ee.getDeviceHeight()));
            touches.put(ee.getId(), new Point(ee.getFracX(), ee.getFracY()));
        } else if (e instanceof EventTouchMove ee) {
            touches.put(ee.getId(), new Point(ee.getFracX(), ee.getFracY()));
        } else if (e instanceof EventTouchCancel ee) {
            touches.remove(ee.getId());
        } else if (e instanceof EventTouchEnd ee) {
            touches.remove(ee.getId());
        } else if (e instanceof EventTouchFrame ee) {
            window.requestFrame();
        }
    }

    @Override
    public void paintImpl(Canvas canvas, int width, int height, float scale) {
        if (touches.isEmpty()) return;

        var capHeight = Example.FONT12.getMetrics().getCapHeight();
        var padding = (int) 8 * scale;

        try (var bg = new Paint().setColor(0x40FFFFFF);
             var fg = new Paint().setColor(0xFF000000)) {
            for (var touch : touches.entrySet()) {
                final int id = touch.getKey();
                final Point pos = touch.getValue();
                final float x = pos.getX() * width;
                final float y = pos.getY() * height;
                canvas.drawCircle(x, y, radius, bg);
                try (var line = TextLine.make(String.valueOf(id), Example.FONT12)) {
                    canvas.drawTextLine(line, x - line.getWidth()/2, y + capHeight/2, fg);
                }
            }
        }
    }
}
