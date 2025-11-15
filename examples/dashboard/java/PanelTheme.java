package io.github.humbleui.jwm.examples;

import io.github.humbleui.jwm.*;
import io.github.humbleui.types.*;
import io.github.humbleui.skija.*;

public class PanelTheme extends Panel {

    public PanelTheme(Window window) {
        super(window);
    }

    @Override
    public void accept(Event e) {
        if (e instanceof EventKey ee && ee.isPressed() && ee.getKey() == Key.Z && ee.isModifierDown(Example.MODIFIER)) {
            ZOrder zOrder = window.getZOrder();
            ZOrder next = zOrder._values[(zOrder.ordinal() + 1) % zOrder._values.length];
            window.setZOrder(next);
        }
    }

    @Override
    public void paintImpl(Canvas canvas, int width, int height, float scale) {
        try (var paint = new Paint()) {
            // sRGB row
            paint.setColor(0xFFFFFFFF);
            canvas.drawString("sRGB", Example.PADDING, Example.PADDING * 2, Example.FONT12, paint);
            float x = width / 2 + Example.PADDING / 2;
            Color4f[] colors = {
                new Color4f(1, 0, 0, 1),  // red
                new Color4f(0, 1, 0, 1),  // green
                new Color4f(0, 0, 1, 1),  // blue
                new Color4f(1, 1, 0, 1),  // yellow
                new Color4f(1, 0, 1, 1),  // magenta
                new Color4f(0, 1, 1, 1)   // cyan
            };
            for (int i = 0; i < colors.length; i++) {
                paint.setColor4f(colors[i], ColorSpace.getSRGB());
                canvas.drawRect(Rect.makeXYWH(x + i * Example.PADDING * 2, Example.PADDING * 0.5f, Example.PADDING * 2, Example.PADDING * 2), paint);
            }

            // Display P3 row
            paint.setColor(0xFFFFFFFF);
            canvas.drawString("Display P3", Example.PADDING, Example.PADDING * 4, Example.FONT12, paint);
            x = width / 2 + Example.PADDING / 2;
            for (int i = 0; i < colors.length; i++) {
                paint.setColor4f(colors[i], ColorSpace.getDisplayP3());
                canvas.drawRect(Rect.makeXYWH(x + i * Example.PADDING * 2, Example.PADDING * 2.5f, Example.PADDING * 2, Example.PADDING * 2), paint);
            }

            paint.setColor(0xFFFFFFFF);
            canvas.drawString("isHighContrast", Example.PADDING, Example.PADDING * 6, Example.FONT12, paint);
            canvas.drawString("" + Theme.isHighContrast(), width / 2 + Example.PADDING / 2, Example.PADDING * 6, Example.FONT12, paint);

            canvas.drawString("isDark", Example.PADDING, Example.PADDING * 8, Example.FONT12, paint);
            canvas.drawString("" + Theme.isDark(), width / 2 + Example.PADDING / 2, Example.PADDING * 8, Example.FONT12, paint);

            canvas.drawString("isInverted", Example.PADDING, Example.PADDING * 10, Example.FONT12, paint);
            canvas.drawString("" + Theme.isInverted(), width / 2 + Example.PADDING / 2, Example.PADDING * 10, Example.FONT12, paint);

            canvas.drawString("zOrder", Example.PADDING, Example.PADDING * 12, Example.FONT12, paint);
            canvas.drawString("" + window.getZOrder(), width / 2 + Example.PADDING / 2, Example.PADDING * 12, Example.FONT12, paint);
        }
    }
}