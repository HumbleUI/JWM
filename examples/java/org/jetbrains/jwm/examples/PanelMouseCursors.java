package org.jetbrains.jwm.examples;

import java.util.*;
import java.util.function.*;
import java.util.stream.*;

import org.jetbrains.jwm.*;
import org.jetbrains.skija.*;

public class PanelMouseCursors extends Panel {
    public Window window;
    public EventMouseMove lastMove = new EventMouseMove(0, 0, 0, 0);
    public Map<MouseCursor, UIRect> rects = new HashMap<>();

    public PanelMouseCursors(Window window) {
        this.window = window;
    }

    @Override
    public void accept(Event e) {
        if (e instanceof EventMouseMove ee) {
            lastMove = ee;
            
            var relX = lastMove.getX() - lastX;
            var relY = lastMove.getY() - lastY;

            for (var entry: rects.entrySet()) {
                if (entry.getValue().contains(relX, relY))
                    window.setMouseCursor(entry.getKey());
            }
        }
    }

    @Override
    public void paintImpl(Canvas canvas, int width, int height, float scale) {
        try (var fg = new Paint().setColor(0x40FFFFFF);
             var hl = new Paint().setColor(0xFFFFFFFF);
             var bg = new Paint().setColor(0x40000000);)
        {
            var capHeight = (int) Example.FONT12.getMetrics().getCapHeight();
            var padding = (int) (5 * scale);
            var x = Example.PADDING;
            var y = Example.PADDING;
            rects.clear();
            for (var cursor: MouseCursor._values) {
                try (var line = TextLine.make(capitalize(cursor.toString()), Example.FONT12);) {
                    if (y + capHeight + 2 * padding > height - Example.PADDING) {
                        x += width / 2 - Example.PADDING / 2;
                        y = Example.PADDING;
                    }
                    var relX = lastMove.getX() - lastX;
                    var relY = lastMove.getY() - lastY;
                    var bounds = UIRect.makeXYWH(x, y, (int) line.getWidth() + 2 * padding, capHeight + 2 * padding);
                    rects.put(cursor, bounds);
                    if (bounds.contains(relX, relY)) {
                        canvas.drawRect(Rect.makeLTRB(bounds.getLeft(), bounds.getTop(), bounds.getRight(), bounds.getBottom()), bg);
                        canvas.drawTextLine(line, x + padding, y + padding + capHeight, hl);
                    } else {
                        canvas.drawTextLine(line, x + padding, y + padding + capHeight, fg);
                    }
                    
                    y += capHeight + 2 * padding;
                }
            }
        }
    }
}
