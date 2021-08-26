package org.jetbrains.jwm.examples;

import java.util.function.*;
import org.jetbrains.jwm.*;
import org.jetbrains.skija.*;

public class PanelScreens extends Panel {
    public EventWindowResize lastResize = new EventWindowResize(0, 0, 0, 0);
    public EventWindowMove lastMove = new EventWindowMove(0, 0);
    public Paint stroke = new Paint().setMode(PaintMode.STROKE).setColor(0x80FFFFFF);
    public Paint fill = new Paint().setColor(0x20FFFFFF);
    public Paint white = new Paint().setColor(0xFFFFFFFF);
    public int screenIdx = -1;

    public PanelScreens(Window window) {
        super(window);
    }

    @Override
    public void accept(Event e) {
        float scale = window.getScreen().getScale();
        if (e instanceof EventKey eventKey) {
            if (eventKey.isPressed() == true && eventKey.isModifierDown(Example.MODIFIER)) {
                switch (eventKey.getKey()) {
                    case DIGIT1 -> {
                        Screen[] screens = App.getScreens();
                        screenIdx = (screenIdx + 1) % screens.length;
                        UIRect bounds = screens[screenIdx].getBounds();
                        window.setWindowPosition(bounds.getLeft() + bounds.getWidth() / 2,
                                                 bounds.getTop() + bounds.getHeight() / 2);
                        window.setWindowSize(bounds.getWidth() / 2,
                                             bounds.getHeight() / 2);
                    }
                    case DIGIT2 ->
                        window.setWindowSize((int) (600 * scale), (int) (500 * scale));
                    case DIGIT3 ->
                        window.setContentSize((int) (600 * scale), (int) (500 * scale));
                }
            }
        } if (e instanceof EventWindowResize ee) {
            lastResize = ee;
            window.requestFrame();
        } else if (e instanceof EventWindowMove ee) {
            lastMove = ee;
            window.requestFrame();
        }
    }

    public void drawRect(Canvas canvas, UIRect rect) {
        canvas.drawRect(Rect.makeXYWH(rect.getLeft(), rect.getTop(), rect.getWidth(), rect.getHeight()), fill);
        canvas.drawRect(Rect.makeXYWH(rect.getLeft(), rect.getTop(), rect.getWidth(), rect.getHeight()), stroke);
    }

    @Override
    public void paintImpl(Canvas canvas, int width, int height, float scale) {
        float minX = 0, minY = 0, maxX = 0, maxY = 0;
        for (var screen: App.getScreens()) {
            var bounds = screen.getBounds();
            minX = Math.min(minX, bounds.getLeft());
            minY = Math.min(minY, bounds.getTop());
            maxX = Math.max(maxX, bounds.getRight());
            maxY = Math.max(maxY, bounds.getBottom());
        }

        canvas.save();
        float scale2 = Math.min((width - Example.PADDING * 2) / (maxX - minX),
                               (height - Example.PADDING * 2) / (maxY - minY));
        canvas.translate(Example.PADDING, Example.PADDING);
        canvas.scale(scale2, scale2);
        canvas.translate(-minX, -minY);
        stroke.setStrokeWidth(1 * scale / scale2);
        for (var screen: App.getScreens()) {
            stroke.setColor(screen.isPrimary() ? 0x80CC3333 : 0x80FFFFFF);
            drawRect(canvas, screen.getBounds());
            drawRect(canvas, screen.getWorkArea());
        }
        UIRect windowRect = window.getWindowRect();
        drawRect(canvas, windowRect);
        drawRect(canvas, window.getContentRectAbsolute());
        stroke.setColor(0x80CC3333);
        drawRect(canvas, UIRect.makeXYWH(lastMove.getWindowLeft(), lastMove.getWindowTop(), lastResize.getWindowWidth(), lastResize.getWindowHeight()));

        canvas.restore();

        // Shortcuts
        try (var bg    = new Paint().setColor(0x40000000);
             var fg    = new Paint().setColor(0xFFFFFFFF);
             var one   = TextLine.make(Platform.CURRENT == Platform.MACOS ? "⌘ 1" : "Ctrl 1", Example.FONT12);
             var two   = TextLine.make(Platform.CURRENT == Platform.MACOS ? "⌘ 2" : "Ctrl 2", Example.FONT12);
             var three = TextLine.make(Platform.CURRENT == Platform.MACOS ? "⌘ 3" : "Ctrl 3", Example.FONT12);)
        {
            var metrics = Example.FONT12.getMetrics();
            var capHeight = metrics.getCapHeight();
            var padding = (int) 8 * scale;
            canvas.save();
            canvas.translate(Example.PADDING, height - Example.PADDING - padding * 2 - capHeight);
            
            canvas.drawRRect(RRect.makeXYWH(0, 0, one.getWidth() + padding * 2, capHeight + padding * 2, 4 * scale), bg);
            canvas.drawTextLine(one, padding, padding + capHeight, fg);
            canvas.translate(padding * 2 + Example.PADDING + one.getWidth(), 0);

            canvas.drawRRect(RRect.makeXYWH(0, 0, two.getWidth() + padding * 2, capHeight + padding * 2, 4 * scale), bg);
            canvas.drawTextLine(two, padding, padding + capHeight, fg);
            canvas.translate(padding * 2 + Example.PADDING + two.getWidth(), 0);

            canvas.drawRRect(RRect.makeXYWH(0, 0, three.getWidth() + padding * 2, capHeight + padding * 2, 4 * scale), bg);
            canvas.drawTextLine(three, padding, padding + capHeight, fg);
            canvas.translate(padding * 2 + Example.PADDING + three.getWidth(), 0);

            canvas.restore();
        }
    }
}