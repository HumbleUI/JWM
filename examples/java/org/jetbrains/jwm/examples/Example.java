package org.jetbrains.jwm.examples;

import org.jetbrains.jwm.*;
import org.jetbrains.skija.*;

import java.util.*;
import java.util.function.*;
import java.util.stream.*;

public class Example implements Consumer<Event> {
    public Window _window;
    public SkijaLayer _layer;
    public Timer timer = new Timer(true);

    public long paintCount = 0;
    public int angle = 0;
    public Font font = new Font(FontMgr.getDefault().matchFamilyStyleCharacter(null, FontStyle.NORMAL, null, "↑".codePointAt(0)), 12);
    public Font font24 = new Font(FontMgr.getDefault().matchFamilyStyle(null, FontStyle.NORMAL), 24);
    public Font font48 = new Font(FontMgr.getDefault().matchFamilyStyle(null, FontStyle.BOLD), 48);
    public EventMouseMove lastMouseMove = null;
    public EventResize lastResize = new EventResize(0, 0);
    public EventWindowMove lastMove = new EventWindowMove(0, 0);
    public Point scroll = new Point(0, 0);

    public String[] variants;
    public int variantIdx = 0;

    public long t0 = System.nanoTime();
    public double[] times = new double[180];
    public int timesIdx = 0;
    public boolean _paused = false;

    public Set<Key> keys = Collections.synchronizedSortedSet(new TreeSet<Key>());
    public Set<MouseButton> buttons = Collections.synchronizedSortedSet(new TreeSet<MouseButton>());

    public Example() {
        variants = Platform.CURRENT == Platform.MACOS
            ? new String[] { "SkijaLayerGL", "macos.SkijaLayerMetal" }
            : new String[] { "SkijaLayerGL" };

        App.makeWindow((window) -> {
            _window = window;
            _window.setEventListener(this);
            changeLayer();
            var scale = _window.getScale();
            _window.move((int) (100 * scale), (int) (100 * scale));
            _window.resize((int) (800 * scale), (int) (600 * scale));
            _window.show();
            _window.requestFrame();
        });
    }

    public void paint() {
        paintCount += 1;

        if (_layer == null)
            return;

        var canvas = _layer.beforePaint();

        canvas.clear(0xFF264653);
        int layer = canvas.save();
        float scale = _window.getScale();
        int width = (int) (_window.getWidth() / scale);
        int height = (int) (_window.getHeight() / scale);
        int halfWidth = width / 2;
        int halfHeight = height / 2;
        canvas.scale(scale, scale);

        try (var paint = new Paint()) {
            // Triangles
            int[] colors = new int[] { 0xFFe76f51, 0xFF2a9d8f, 0xFFe9c46a };
            canvas.drawTriangles(new Point[] { new Point(10, 10), new Point(200, 10), new Point (10, 200) }, colors, paint);
            canvas.drawTriangles(new Point[] { new Point(width - 10, 10), new Point(width - 200, 10), new Point(width - 10, 200) }, colors, paint);
            canvas.drawTriangles(new Point[] { new Point(10, height - 10), new Point(200, height - 10), new Point (10, height - 200) }, colors, paint);
            // canvas.drawTriangles(new Point[] { new Point(width - 10, height - 10), new Point(width - 200, height - 10), new Point(width - 10, height - 200) }, colors, paint);

            // Pill
            canvas.save();
            canvas.translate(width / 2, height / 2);
            paint.setColor(0xFFFFFFFF);
            canvas.drawCircle(0, 0, 100, paint);
            angle = (angle + 3) % 360;
            canvas.rotate(angle);
            paint.setColor(0xFF264653);
            canvas.drawRect(Rect.makeXYWH(-7, -100, 14, 200), paint);
            canvas.restore();

            // Cursor
            paint.setColor(0x20FFFFFF);
            if (lastMouseMove != null) {
                var x = (int) (lastMouseMove.getX() / _window.getScale());
                var y = (int) (lastMouseMove.getY() / _window.getScale());
                canvas.drawRect(Rect.makeXYWH(0, y - 1, width, 2), paint);
                canvas.drawRect(Rect.makeXYWH(x - 1, 0, 2, height), paint);

                String text = x + ", " + y;
                for (var button: MouseButton.values())
                    if (lastMouseMove.isButtonDown(button))
                        text += " + " + button;
                for (var modifier: KeyModifier.values())
                    if (lastMouseMove.isModifierDown(modifier))
                        text += " + " + modifier;
                canvas.drawString(text, x + 3, y - 5, font, paint);
            }

            // paint.setColor(0x80FFFFFF).setMode(PaintMode.STROKE).setStrokeWidth(1);
            // var radius = (float) Math.hypot(x - lastX, y - lastY);
            // canvas.drawCircle(x, y, radius, paint);
            // canvas.drawCircle(x, y, radius * 2, paint);

            // Colored bars
            var barProp = 0.2f;
            var barSize = 2;
            paint.setColor(0xFFe76f51);
            canvas.drawRect(Rect.makeXYWH(0, halfHeight - halfHeight * 0.2f, barSize, height * 0.2f), paint);
            paint.setColor(0xFF2a9d8f);
            canvas.drawRect(Rect.makeXYWH(halfWidth - halfWidth * 0.2f, 0, width * 0.2f, barSize), paint);
            paint.setColor(0xFFe9c46a);
            canvas.drawRect(Rect.makeXYWH(width - barSize, halfHeight - halfHeight * 0.2f, barSize, height * 0.2f), paint);
            paint.setColor(0xFFFFFFFF);
            canvas.drawRect(Rect.makeXYWH(halfWidth - halfWidth * 0.2f, height - barSize, width * 0.2f, barSize), paint);
        }

        // Scroll
        try (var paint = new Paint().setMode(PaintMode.STROKE).setStrokeWidth(2).setColor(0x80FFFFFF)) {
            for (int x = (int) Math.ceil(-scroll.getX() / 50f - 1) * 50; x + scroll.getX() < width; x += 50) {
                canvas.drawLine(scroll.getX() + x, halfHeight - 2, scroll.getX() + x, halfHeight + 2, paint);
                canvas.drawLine(scroll.getX() + x + 25, halfHeight - 4, scroll.getX() + x + 25, halfHeight + 4, paint);
            }

            for (int y = (int) Math.ceil(-scroll.getY() / 50f - 1) * 50; y + scroll.getY() < height; y += 50) {
                canvas.drawLine(halfWidth - 2, scroll.getY() + y, halfWidth + 2, scroll.getY() + y, paint);
                canvas.drawLine(halfWidth - 4, scroll.getY() + y + 25, halfWidth + 4, scroll.getY() + y + 25, paint);
            }
        }

        // Keys and Buttons
        if (!keys.isEmpty() || !buttons.isEmpty()) {
            try (var paint = new Paint()) {
                String text = String.join(" + ", Stream.concat(keys.stream().map(Key::getName), buttons.stream().map(Object::toString)).collect(Collectors.toList()));
                try (var line = TextLine.make(text, font24);) {
                    var capHeight = font24.getMetrics().getCapHeight();
                    paint.setColor(0x40000000);
                    canvas.drawRRect(RRect.makeXYWH((width - line.getWidth()) / 2 - 8, height - capHeight - 16 - 20, line.getWidth() + 16, capHeight + 16, 4), paint);
                    paint.setColor(0xFFFFFFFF);
                    canvas.drawTextLine(line, (width - line.getWidth()) / 2, height - 20 - 8, paint);
                }
            }
        }

        // VSync
        try (var paint = new Paint()) {
            canvas.save();
            canvas.translate(width - (8 + 180 + 8 + 8), height - (8 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 32 + 8 + 8) - 68);
            paint.setColor(0xFFE0E0E0);
            canvas.drawRRect(RRect.makeXYWH(0, 0, 196, 60, 4), paint);
            paint.setColor(angle % 2 == 0 ? 0xFFEF8784 : 0xFFA1FCFE);
            var bounds = font48.measureText("VSYNC");
            canvas.drawString("VSYNC", (196 - bounds.getWidth()) / 2, (60 + font48.getMetrics().getCapHeight()) / 2, font48, paint);
            canvas.restore();
        }

        // HUD
        try (var paint = new Paint()) {
            canvas.save();
            canvas.translate(width - (8 + 180 + 8 + 8), height - (8 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 32 + 8 + 8));

            // bg
            paint.setColor(0x40000000);
            canvas.drawRRect(RRect.makeXYWH(0, 0, 8 + 180 + 8, 8 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 32 + 8, 4), paint);
            canvas.translate(8, 8);

            // Dimensions
            paint.setColor(0xFFFFFFFF);
            canvas.drawString("Pos: " + _window.getLeft() + "×" + _window.getTop() + " (" + lastMove.getLeft() + "×" + lastMove.getTop() + ")", 0, 12, font, paint);
            canvas.translate(0, 24);
            canvas.drawString("Size: " + _window.getWidth() + "×" + _window.getHeight() + " (" + lastResize.getWidth() + "×" + lastResize.getHeight() + ")", 0, 12, font, paint);
            canvas.translate(0, 24);
            canvas.drawString("Scale: " + _window.getScale(), 0, 12, font, paint);
            canvas.translate(0, 24);

            // Variant
            paint.setColor(0x80000000);
            canvas.drawRRect(RRect.makeXYWH(0, 0, 16, 16, 2), paint);
            paint.setColor(0xFFFFFFFF);
            canvas.drawString("↓↑", 0, 12, font, paint);
            canvas.drawString(variants[variantIdx], 24, 12, font, paint);
            canvas.translate(0, 24);

            paint.setColor(0x80000000);
            canvas.drawRRect(RRect.makeXYWH(0, 0, 16, 16, 2), paint);
            paint.setColor(0xFFFFFFFF);
            canvas.drawString("P", 3, 12, font, paint);
            canvas.drawString(_paused ? "Paused" : "Not paused", 24, 12, font, paint);
            canvas.translate(0, 24);

            paint.setColor(0x80000000);
            canvas.drawRRect(RRect.makeXYWH(0, 0, 16, 16, 2), paint);
            paint.setColor(0xFFFFFFFF);
            canvas.drawString("N", 3, 12, font, paint);
            canvas.drawString("New window", 24, 12, font, paint);
            canvas.translate(0, 24);

            paint.setColor(0x80000000);
            canvas.drawRRect(RRect.makeXYWH(0, 0, 16, 16, 2), paint);
            paint.setColor(0xFFFFFFFF);
            canvas.drawString("W", 3, 12, font, paint);
            canvas.drawString("Close window", 24, 12, font, paint);
            canvas.translate(0, 24);

            // FPS
            long t1 = System.nanoTime();
            times[timesIdx] = (t1 - t0) / 1000000.0;
            t0 = t1;
            timesIdx = (timesIdx + 1) % times.length;
            int frames = 0;
            double time = 0;
            for (int i = 0; i < times.length; ++i) {
                var idx = (timesIdx - i + times.length) % times.length;
                if (times[idx] > 0) {
                    time += times[idx];
                    frames++;
                }
                if (time > 1000)
                    break;
            }
            String fps = String.format("%.01f", (frames / time * 1000));
            canvas.drawString("FPS: " + fps, 0, 12, font, paint);
            canvas.translate(0, 24);

            // FPS graph
            paint.setColor(0x4033cc33);
            canvas.drawRRect(RRect.makeXYWH(-2, -2, 184, 36, 2), paint);
            paint.setColor(0xFF33CC33);
            for (int i = 0; i < times.length; ++i) {
                var idx = (timesIdx + i) % times.length;
                canvas.drawRect(Rect.makeXYWH(i, 32 - (float) times[idx], 1, (float) times[idx]), paint);
            }

            paint.setColor(0x80000000);
            canvas.drawRect(Rect.makeXYWH(-2, 32 - 17, times.length + 4, 1), paint);
            canvas.drawRect(Rect.makeXYWH(-2, 32 - 8, times.length + 4, 1), paint);
            canvas.restore();
        }

        canvas.restoreToCount(layer);

        _layer.afterPaint();
    }

    public void changeLayer() {
        if (_layer != null)
            _layer.close();

        String className = "org.jetbrains.jwm.examples." + variants[variantIdx];
        try {
            _layer = (SkijaLayer) Example.class.forName(className).getDeclaredConstructor().newInstance();
        } catch (Exception e) {
            System.err.println("Failed to create class " + className);
            e.printStackTrace();
            _layer = null;
        }

        _layer.attach(_window);
        _layer.reconfigure();
        _layer.resize(_window.getWidth(), _window.getHeight());
    }

    @Override
    public void accept(Event e) {
        if (e instanceof EventReconfigure) {
            _layer.reconfigure();
            accept(new EventResize(_window.getWidth(), _window.getHeight()));
        } else if (e instanceof EventResize) {
            lastResize = (EventResize) e;
            _layer.resize(lastResize.getWidth(), lastResize.getHeight());
            paint();
        } else if (e instanceof EventTextInput) {
            EventTextInput eti = (EventTextInput) e;
            System.out.println("Input: " + eti.getText());
        } else if (e instanceof EventMouseButton) {
            EventMouseButton ee = (EventMouseButton) e;
            if (ee.isPressed())
                buttons.add(ee.getButton());
            else
                buttons.remove(ee.getButton());
        } else if (e instanceof EventMouseMove) {
            lastMouseMove = (EventMouseMove) e;
        } else if (e instanceof EventKeyboard) {
            EventKeyboard eventKeyboard = (EventKeyboard) e;
            if (eventKeyboard.isPressed() == true) {
                if (eventKeyboard.getKey() == Key.P) {
                    _paused = !_paused;
                    if (!_paused)
                        _window.requestFrame();
                } else if (eventKeyboard.getKey() == Key.N) {
                    new Example();
                } else if (eventKeyboard.getKey() == Key.W || eventKeyboard.getKey() == Key.ESCAPE) {
                    accept(EventClose.INSTANCE);
                } else if (eventKeyboard.getKey() == Key.DOWN) { // ↓
                    variantIdx = (variantIdx + 1) % variants.length;
                    changeLayer();
                } else if (eventKeyboard.getKey() == Key.UP) { // ↑
                    variantIdx = (variantIdx + variants.length - 1) % variants.length;
                    changeLayer();
                }

                keys.add(eventKeyboard.getKey());
            } else {
                keys.remove(eventKeyboard.getKey());
            }
        } else if (e instanceof EventScroll) {
            var ee = (EventScroll) e;
            scroll = scroll.offset(ee.getDeltaX(), ee.getDeltaY());
        } else if (e instanceof EventWindowMove) {
            lastMove = (EventWindowMove) e;
        } else if (e instanceof EventFrame) {
            paint();
            if (!_paused)
                _window.requestFrame();
//             System.out.println((System.nanoTime() - t0) + " PaintEvent");
//             timer.schedule(new TimerTask() {
//                 public void run() {
//                     App.runOnUIThread(() -> { System.out.println((System.nanoTime() - t0) + " Paint"); paint(); });
//                 }
//             }, 10);
        } else if (e instanceof EventClose) {
            _layer.close();
            _window.close();
            if (App._windows.size() == 0)
                App.terminate();
        }
    }

    public static void main(String[] args) {
        App.init();

        System.out.println("Screens:");
        for (Screen screen: App.getScreens())
            System.out.println("  " + screen);

        new Example();
        App.start();
    }
}
