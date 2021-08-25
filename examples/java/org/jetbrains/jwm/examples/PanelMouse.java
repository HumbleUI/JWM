package org.jetbrains.jwm.examples;

import java.util.*;
import java.util.function.*;
import java.util.stream.*;

import org.jetbrains.jwm.*;
import org.jetbrains.skija.*;

public class PanelMouse extends Panel {
    public EventMouseMove lastMouseMove = null;
    public Point scroll = new Point(0, 0);
    public List<MouseButton> buttons = Collections.synchronizedList(new ArrayList<MouseButton>());

    @Override
    public void accept(Event e) {
        if (e instanceof EventMouseMove ee) {
            lastMouseMove = ee;
        } else if (e instanceof EventMouseScroll ee) {
            scroll = scroll.offset(ee.getDeltaX() * lastScale, ee.getDeltaY() * lastScale);
        } else if (e instanceof EventMouseButton ee) {
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
        var capHeight = Example.FONT12.getMetrics().getCapHeight();

        // position
        if (lastMouseMove != null) {
            try (var paint = new Paint().setColor(0x40FFFFFF)) {
                var x = lastMouseMove.getX() - lastX;
                var y = lastMouseMove.getY() - lastY;
                canvas.drawRect(Rect.makeXYWH(0, y - 1 * scale, width, 2 * scale), paint);
                canvas.drawRect(Rect.makeXYWH(x - 1 * scale, 0, 2 * scale, height), paint);

                canvas.save();
                canvas.translate(x + 3 * scale, y - 5 * scale);
                canvas.drawString(x + ", " + y, 0, 0, Example.FONT12, paint);
                canvas.translate(0, 10 * scale + capHeight);
                for (var button: MouseButton.values())
                    if (lastMouseMove.isButtonDown(button)) {
                        canvas.drawString(capitalize(button.toString()), 0, 0, Example.FONT12, paint);
                        canvas.translate(0, 2 * capHeight);
                    }
                for (var modifier: KeyModifier.values())
                    if (lastMouseMove.isModifierDown(modifier)) {
                        canvas.drawString(capitalize(modifier.toString()), 0, 0, Example.FONT12, paint);
                        canvas.translate(0, 2 * capHeight);
                    }
                canvas.restore();
            }
        }

        // scroll
        int halfWidth = width / 2;
        int halfHeight = height / 2;
        int step = (int) (25 * scale);
        int halfStep = step / 2;

        try (var paint = new Paint().setMode(PaintMode.STROKE).setStrokeWidth(2 * scale).setColor(0x40FFFFFF)) {
            for (int x = (int) Math.ceil(-scroll.getX() / step - 1) * step; x + scroll.getX() < width; x += step) {
                canvas.drawLine(scroll.getX() + x, 0, scroll.getX() + x, 5 * scale, paint);
                canvas.drawLine(scroll.getX() + x + halfStep, 0, scroll.getX() + x + halfStep, 8 * scale, paint);
            }

            for (int y = (int) Math.ceil(-scroll.getY() / step - 1) * step; y + scroll.getY() < height; y += step) {
                canvas.drawLine(0, scroll.getY() + y, 5 * scale, scroll.getY() + y, paint);
                canvas.drawLine(0, scroll.getY() + y + halfStep, 8 * scale, scroll.getY() + y + halfStep, paint);
            }
        }

        // buttons

        var lines = Arrays.stream(MouseButton._values).map((button) -> TextLine.make(capitalize(button.toString()), Example.FONT12)).collect(Collectors.toList());
        try (var paint = new Paint();) {
            

            var padding = (int) 5 * scale;
            int y = Example.PADDING;
            for (var button: MouseButton._values) {
                try (var line = TextLine.make(capitalize(button.toString()), Example.FONT12); ) {
                    var pressed = buttons.contains(button);
                    if (pressed) {
                        paint.setColor(0x40000000);
                        canvas.drawRRect(RRect.makeXYWH(Example.PADDING, y, line.getWidth() + 2 * padding, capHeight + 2 * padding, 4 * scale), paint);
                    }

                    paint.setColor(pressed ? 0xFFFFFFFF : 0x40FFFFFF);
                    canvas.drawTextLine(line, Example.PADDING + padding, y + capHeight + padding, paint);

                    y += capHeight + 3 * padding;
                }
            }
        }
    }
}
