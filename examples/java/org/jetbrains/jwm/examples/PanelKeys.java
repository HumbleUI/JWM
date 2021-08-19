package org.jetbrains.jwm.examples;

import java.util.*;
import java.util.function.*;
import java.util.stream.*;

import org.jetbrains.jwm.*;
import org.jetbrains.skija.*;

public class PanelKeys extends Panel {
    public List<Key> keys = Collections.synchronizedList(new ArrayList<Key>());

    @Override
    public void accept(Event e) {
        if (e instanceof EventKey ee) {
            var key = ee.getKey();
            if (ee.isPressed() == true) {
                if (!keys.contains(key))
                    keys.add(key);
            } else
                keys.remove(key);
        }
    }

    @Override
    public void paintImpl(Canvas canvas, int width, int height, float scale) {
        var lines = keys.stream().map((key) -> TextLine.make(key.getName(), Example.FONT24)).collect(Collectors.toList());
        try (var paint = new Paint();) {
            var capHeight = Example.FONT24.getMetrics().getCapHeight();

            int x = Example.PADDING, y = Example.PADDING;
            for (var line: lines) {
                if (x > Example.PADDING && x + line.getWidth() + 2 * Example.PADDING > width - Example.PADDING) {
                    x = Example.PADDING;
                    y += capHeight + 3 * Example.PADDING;
                }

                paint.setColor(0x40000000);
                canvas.drawRRect(RRect.makeXYWH(x, y, line.getWidth() + 2 * Example.PADDING, capHeight + 2 * Example.PADDING, 4 * scale), paint);
                paint.setColor(0xFFFFFFFF);
                canvas.drawTextLine(line, x + Example.PADDING, y + capHeight + Example.PADDING, paint);

                x += line.getWidth() + 3 * Example.PADDING;
                line.close();
            }
        }
    }
}
