package org.jetbrains.jwm.examples;

import java.util.*;
import java.util.function.*;
import java.util.stream.*;

import org.jetbrains.jwm.*;
import org.jetbrains.skija.*;

public class PanelMouseButtons extends Panel {
    public List<MouseButton> buttons = Collections.synchronizedList(new ArrayList<MouseButton>());

    @Override
    public void accept(Event e) {
        if (e instanceof EventMouseButton ee) {
            var button = ee.getButton();
            if (ee.isPressed() == true) {
                if (!buttons.contains(button))
                    buttons.add(button);
            } else
                buttons.remove(button);
        }
    }

    @Override
    public void paintImpl(Canvas canvas, int width, int height, float scale) {
        var lines = buttons.stream().map((button) -> TextLine.make(button.toString(), Example.FONT24)).collect(Collectors.toList());
        try (var paint = new Paint();) {
            var capHeight = Example.FONT24.getMetrics().getCapHeight();

            int y = Example.PADDING;
            for (var line: lines) {
                paint.setColor(0x40000000);
                canvas.drawRRect(RRect.makeXYWH(Example.PADDING, y, line.getWidth() + 2 * Example.PADDING, capHeight + 2 * Example.PADDING, 4 * scale), paint);
                paint.setColor(0xFFFFFFFF);
                canvas.drawTextLine(line, 2 * Example.PADDING, y + capHeight + Example.PADDING, paint);

                y += capHeight + 3 * Example.PADDING;
                line.close();
            }
        }
    }
}
