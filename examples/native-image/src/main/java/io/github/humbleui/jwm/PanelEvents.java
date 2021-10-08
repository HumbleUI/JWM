package io.github.humbleui.jwm.examples;

import java.util.*;
import io.github.humbleui.jwm.*;
import org.jetbrains.skija.*;

public class PanelEvents extends Panel {
    public List<Event> events = new ArrayList<>();

    public PanelEvents(Window window) {
        super(window);
    }

    @Override
    public void accept(Event e) {
        while (events.size() > 19)
            events.remove(0);
        if (!(e instanceof EventFrame)) {
            events.add(e);
            window.requestFrame();
        }
    }

    @Override
    public void paintImpl(Canvas canvas, int width, int height, float scale) {
        try (var paint = new Paint();) {
            paint.setColor(0xFFFFFFFF);
            var metrics = Example.FONT12.getMetrics();
            canvas.save();
            canvas.translate(Example.PADDING, height - Example.PADDING - metrics.getDescent());
            for (int i = events.size() - 1; i >= 0; --i) {
                var event = events.get(i);
                canvas.drawString(event.toString(), 0, 0, Example.FONT12, paint);
                canvas.translate(0, -metrics.getCapHeight() - 8 * scale);
            }
            canvas.restore();
        }
    }
}