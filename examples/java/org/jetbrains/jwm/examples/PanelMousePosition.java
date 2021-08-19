package org.jetbrains.jwm.examples;

import java.util.*;
import java.util.function.*;
import java.util.stream.*;

import org.jetbrains.jwm.*;
import org.jetbrains.skija.*;

public class PanelMousePosition extends Panel {
    public EventMouseMove lastMouseMove = null;

    @Override
    public void accept(Event e) {
        if (e instanceof EventMouseMove ee) {
            lastMouseMove = ee;
        }
    }

    @Override
    public void paintImpl(Canvas canvas, int width, int height, float scale) {
        int halfWidth = width / 2;
        int halfHeight = height / 2;
        int step = (int) (50 * scale);
        int halfStep = step / 2;

        if (lastMouseMove != null) {
            try (var paint = new Paint().setColor(0x40FFFFFF)) {
                var x = lastMouseMove.getX() - lastX;
                var y = lastMouseMove.getY() - lastY;
                canvas.drawRect(Rect.makeXYWH(0, y - 1 * scale, width, 2 * scale), paint);
                canvas.drawRect(Rect.makeXYWH(x - 1 * scale, 0, 2 * scale, height), paint);

                String text = x + ", " + y;
                for (var button: MouseButton.values())
                    if (lastMouseMove.isButtonDown(button))
                        text += " + " + button;
                for (var modifier: KeyModifier.values())
                    if (lastMouseMove.isModifierDown(modifier))
                        text += " + " + modifier;
                canvas.drawString(text, (int) (x + 3 * scale), (int) (y - 5 * scale), Example.FONT12, paint);
            }
        }
    }
}
