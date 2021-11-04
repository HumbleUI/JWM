package io.github.humbleui.jwm.examples;

import io.github.humbleui.jwm.*;
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
            paint.setColor(0xFFFFFFFF);
            canvas.drawString("isHighContrast", Example.PADDING, Example.PADDING * 2, Example.FONT12, paint);
            canvas.drawString("" + Theme.isHighContrast(), width / 2 + Example.PADDING / 2, Example.PADDING * 2, Example.FONT12, paint);

            canvas.drawString("isDark", Example.PADDING, Example.PADDING * 4, Example.FONT12, paint);
            canvas.drawString("" + Theme.isDark(), width / 2 + Example.PADDING / 2, Example.PADDING * 4, Example.FONT12, paint);

            canvas.drawString("isInverted", Example.PADDING, Example.PADDING * 6, Example.FONT12, paint);
            canvas.drawString("" + Theme.isInverted(), width / 2 + Example.PADDING / 2, Example.PADDING * 6, Example.FONT12, paint);

            canvas.drawString("zOrder", Example.PADDING, Example.PADDING * 8, Example.FONT12, paint);
            canvas.drawString("" + window.getZOrder(), width / 2 + Example.PADDING / 2, Example.PADDING * 8, Example.FONT12, paint);            
        }
    }
}