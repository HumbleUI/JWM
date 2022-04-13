package io.github.humbleui.jwm.examples;

import io.github.humbleui.jwm.*;
import io.github.humbleui.jwm.skija.*;
import io.github.humbleui.skija.*;
import io.github.humbleui.types.*;

import java.util.*;
import java.util.function.*;

public class Example implements Consumer<Event> {
    public Window window;

    public Example() {
        window = App.makeWindow();
        window.setEventListener(this);
        window.setTitle("Empty");
        window.setLayer(new LayerGLSkija());

        var screen = App.getPrimaryScreen();
        var scale = screen.getScale();
        var bounds = screen.getWorkArea();

        window.setWindowSize((int) (300 * scale), (int) (600 * scale));
        window.setWindowPosition((int) (300 * scale), (int) (200 * scale));
        window.setVisible(true);
    }

    public void paint(Canvas canvas, int width, int height) {
        float scale = window.getScreen().getScale();
        canvas.clear(0xFF264653);
        try (var paint = new Paint()) {
            paint.setColor(0xFFe76f51);

            canvas.drawRect(Rect.makeXYWH(10 * scale, 10 * scale, 10 * scale, 10 * scale), paint);
            canvas.drawRect(Rect.makeXYWH(width - 20 * scale, 10 * scale, 10 * scale, 10 * scale), paint);
            canvas.drawRect(Rect.makeXYWH(10 * scale, height - 20 * scale, 10 * scale, 10 * scale), paint);
            canvas.drawRect(Rect.makeXYWH(width - 20 * scale, height - 20 * scale, 10 * scale, 10 * scale), paint);
            canvas.drawRect(Rect.makeXYWH(width / 2 - 5 * scale, height / 2 - 5 * scale, 10 * scale, 10 * scale), paint);
        }
    }

    @Override
    public void accept(Event e) {
        if (e instanceof EventWindowClose) {
            if (App._windows.size() == 0)
                App.terminate();
            return;
        } else if (e instanceof EventFrameSkija ee) {
            Surface s = ee.getSurface();
            paint(s.getCanvas(), s.getWidth(), s.getHeight());
        } else if (e instanceof EventWindowCloseRequest) {
            window.close();
        }
    }

    public static void main(String[] args) {
        App.start(() -> {
            new Example();
        });
    }
}