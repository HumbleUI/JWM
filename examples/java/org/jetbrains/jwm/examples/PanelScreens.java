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
    public Window window;
    public int screenIdx = -1;

    public PanelScreens(Window window) {
        this.window = window;
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
                        Screen screen = screens[screenIdx];
                        System.out.println("Screen #" + screenIdx + " pos: " + (screen.getX() + screen.getWidth() / 2) + ", " + (screen.getY() + screen.getHeight() / 2) + " for: " + screen);
                        window.setWindowPosition(screen.getX() + screen.getWidth() / 2,
                                                 screen.getY() + screen.getHeight() / 2);
                        window.setWindowSize(screen.getWidth() / 2,
                                             screen.getHeight() / 2);
                    }
                    case DIGIT2 ->
                        window.setWindowSize((int) (600 * scale), (int) (500 * scale));
                    case DIGIT3 ->
                        window.setContentSize((int) (600 * scale), (int) (500 * scale));
                }
            }
        } if (e instanceof EventWindowResize ee) {
            lastResize = ee;
        } else if (e instanceof EventWindowMove ee) {
            lastMove = ee;
        }
    }

    public void drawRect(Canvas canvas, int left, int top, int width, int height) {
        canvas.drawRect(Rect.makeXYWH(left, top, width, height), fill);
        canvas.drawRect(Rect.makeXYWH(left, top, width, height), stroke);
    }

    @Override
    public void paintImpl(Canvas canvas, int width, int height, float scale) {
        float minX = 0, minY = 0, maxX = 0, maxY = 0;
        for (var screen: App.getScreens()) {
            minX = Math.min(minX, screen.getX());
            minY = Math.min(minY, screen.getY());
            maxX = Math.max(maxX, screen.getX() + screen.getWidth());
            maxY = Math.max(maxY, screen.getY() + screen.getHeight());
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
            drawRect(canvas, screen.getX(), screen.getY(), screen.getWidth(), screen.getHeight());
        }
        stroke.setColor(0x80FFFFFF);
        drawRect(canvas, lastMove.getWindowLeft(), lastMove.getWindowTop(), lastResize.getWindowWidth(), lastResize.getWindowHeight());
        UIRect contentRect = window.getContentRect();
        drawRect(canvas,
                 lastMove.getWindowLeft() + contentRect.getLeft(),
                 lastMove.getWindowTop() + contentRect.getTop(),
                 contentRect.getWidth(),
                 contentRect.getHeight());

        stroke.setColor(0x80CC3333);
        UIRect windowRect = window.getWindowRect();
        canvas.drawRect(Rect.makeXYWH(windowRect.getLeft(),
                                      windowRect.getTop(),
                                      windowRect.getWidth(),
                                      windowRect.getHeight()), stroke);

        canvas.restore();

        // Shortcuts
        try (var bg    = new Paint().setColor(0x40000000);
             var fg    = new Paint().setColor(0xFFFFFFFF);
             var one   = TextLine.make(Platform.CURRENT == Platform.MACOS ? "⌘ 1" : "Ctrl 1", Example.FONT12);
             var two   = TextLine.make(Platform.CURRENT == Platform.MACOS ? "⌘ 2" : "Ctrl 2", Example.FONT12);
             var three = TextLine.make(Platform.CURRENT == Platform.MACOS ? "⌘ 3" : "Ctrl 3", Example.FONT12);)
        {
            var metrics = Example.FONT12.getMetrics();
            var padding = (int) 5 * scale;
            canvas.save();
            canvas.translate(Example.PADDING, height - Example.PADDING - padding * 2 - one.getHeight());
            
            canvas.drawRRect(RRect.makeXYWH(0, 0, one.getWidth() + padding * 2, one.getHeight() + padding * 2, 4 * scale), bg);
            canvas.drawTextLine(one, padding, padding - metrics.getAscent(), fg);
            canvas.translate(padding * 2 + Example.PADDING + one.getWidth(), 0);

            canvas.drawRRect(RRect.makeXYWH(0, 0, two.getWidth() + padding * 2, two.getHeight() + padding * 2, 4 * scale), bg);
            canvas.drawTextLine(two, padding, padding - metrics.getAscent(), fg);
            canvas.translate(padding * 2 + Example.PADDING + two.getWidth(), 0);

            canvas.drawRRect(RRect.makeXYWH(0, 0, three.getWidth() + padding * 2, three.getHeight() + padding * 2, 4 * scale), bg);
            canvas.drawTextLine(three, padding, padding - metrics.getAscent(), fg);
            canvas.translate(padding * 2 + Example.PADDING + three.getWidth(), 0);

            canvas.restore();
        }
    }
}