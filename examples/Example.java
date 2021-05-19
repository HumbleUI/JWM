package org.jetbrains.jwm.examples;

import java.util.*;
import java.util.function.*;
import lombok.*;
import org.jetbrains.annotations.*;
import org.jetbrains.jwm.*;
import org.jetbrains.skija.*;

public class Example implements Consumer<Event> {
    public Window _window;
    public SkijaLayer _layer;
    public Timer timer = new Timer(true);

    public int angle = 0;
    public Font font = new Font(FontMgr.getDefault().matchFamilyStyleCharacter(null, FontStyle.NORMAL, null, "↑".codePointAt(0)), 12);
    public Font font48 = new Font(FontMgr.getDefault().matchFamilyStyle(null, FontStyle.BOLD), 48);
    public int x = 0, y = 0, lastX = 0, lastY = 0;

    public String[] variants = new String[] {
        "Metal",
        "OpenGL",
    };
    public int variantIdx = 0;

    public long t0 = System.nanoTime();
    public double[] times = new double[180];
    public int timesIdx = 0;

    public Example() {
        _window = App.makeWindow();
        _window.setEventListener(this);
        changeLayer();
        _window.show();
    }

    public void paint() {
        if (_layer == null)
            return;

        var canvas = _layer.beforePaint();

        canvas.clear(0xFF264653);
        int layer = canvas.save();
        float scale = _window.getScale();
        int width = (int) (_window.getWidth() / scale);
        int height = (int) (_window.getHeight() / scale);
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
            canvas.drawRect(Rect.makeXYWH(0, y - 1, width, 2), paint);
            canvas.drawRect(Rect.makeXYWH(x - 1, 0, 2, height), paint);

            // paint.setColor(0x80FFFFFF).setMode(PaintMode.STROKE).setStrokeWidth(1);
            // var radius = (float) Math.hypot(x - lastX, y - lastY);
            // canvas.drawCircle(x, y, radius, paint);
            // canvas.drawCircle(x, y, radius * 2, paint);
        }

        // VSync
        try (var paint = new Paint()) {
            canvas.save();
            canvas.translate(width / 2 - 100, height - 120);
            paint.setColor(0xFFE0E0E0);
            canvas.drawRRect(RRect.makeXYWH(0, 0, 200, 80, 4), paint);
            paint.setColor(angle % 2 == 0 ? 0xFFEF8784 : 0xFFA1FCFE);
            var bounds = font48.measureText("VSYNC");
            canvas.drawString("VSYNC", (200 - bounds.getWidth()) / 2, (80 + font48.getMetrics().getCapHeight()) / 2, font48, paint);
            canvas.restore();
        }

        // HUD
        try (var paint = new Paint()) {
            canvas.save();
            canvas.translate(width - (8 + 180 + 8 + 8), height - (8 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 32 + 8 + 8));

            // bg
            paint.setColor(0x40000000);
            canvas.drawRRect(RRect.makeXYWH(0, 0, 8 + 180 + 8, 8 + 24 + 24 + 24 + 24 + 24 + 24 + 24 + 32 + 8, 4), paint);
            canvas.translate(8, 8);

            // Dimensions
            paint.setColor(0xFFFFFFFF);
            canvas.drawString("Position: " + _window.getLeft() + "×" + _window.getTop(), 0, 12, font, paint);
            canvas.translate(0, 24);
            canvas.drawString("Size: " + _window.getWidth() + "×" + _window.getHeight(), 0, 12, font, paint);
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

        if ("Metal".equals(variants[variantIdx]))
            _layer = new SkijaLayerMetal();
        else if ("OpenGL".equals(variants[variantIdx]))
            _layer = new SkijaLayerGL();

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
            EventResize er = (EventResize) e;
            _layer.resize(er.getWidth(), er.getHeight());
            paint();
        } else if (e instanceof EventMouseMove) {
            lastX = x;
            lastY = y;
            x = (int) (((EventMouseMove) e).getX() / _window.getScale());
            y = (int) (((EventMouseMove) e).getY() / _window.getScale());
        } else if (e instanceof EventKeyboard) {
            EventKeyboard eventKeyboard = (EventKeyboard) e;
            if (eventKeyboard.isPressed() == true) {
                if (eventKeyboard.getKeyCode() == 45) { // n
                    new Example();
                } else if (eventKeyboard.getKeyCode() == 13 || eventKeyboard.getKeyCode() == 53) { // w || Esc
                    accept(EventClose.INSTANCE);
                } else if (eventKeyboard.getKeyCode() == 125) { // ↓
                    variantIdx = (variantIdx + 1) % variants.length;
                    changeLayer();
                } else if (eventKeyboard.getKeyCode() == 126) { // ↑
                    variantIdx = (variantIdx + variants.length - 1) % variants.length;
                    changeLayer();
                } else 
                    System.out.println("Key pressed: " + eventKeyboard.getKeyCode());
            }
        } else if (e instanceof EventFrame) {
            paint();
            // System.out.println((System.currentTimeMillis() - tt) + " PaintEvent");
            // timer.schedule(new TimerTask() {
            //     public void run() {
            //         App.runOnUIThread(() -> { System.out.println((System.currentTimeMillis() - tt) + " Paint"); paint(); });
            //     }
            // }, 10);
        } else if (e instanceof EventClose) {
            _layer.close();
            _window.close();
            if (App._windows.size() == 0)
                App.terminate();
        }
    }

    public static void main(String[] args) {
        App.init();
        new Example();
        App.start();
    }
}
