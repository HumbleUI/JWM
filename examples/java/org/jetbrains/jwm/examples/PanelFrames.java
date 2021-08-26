package org.jetbrains.jwm.examples;

import java.util.*;
import java.util.concurrent.*;
import java.util.function.*;
import java.util.stream.*;

import org.jetbrains.jwm.*;
import org.jetbrains.skija.*;

public class PanelFrames extends Panel {
    public boolean vsyncColor = false;
    public Map<String, Integer> counters = new ConcurrentHashMap<>();

    public long t0 = System.nanoTime();
    public double[] times = new double[180];
    public int timesIdx = 0;

    public void bumpCounter(String reason) {
        counters.merge(reason, 1, Integer::sum);
    }

    @Override
    public void paintImpl(Canvas canvas, int width, int height, float scale) {
        try (var paint = new Paint();) {
            paint.setColor(0xFFFFFFFF);
            var metrics = Example.FONT12.getMetrics();

            // Paint counters
            canvas.save();
            canvas.translate(Example.PADDING, Example.PADDING - metrics.getAscent());
            for (var entry: counters.entrySet()) {
                canvas.drawString(entry.getKey() + ": " + entry.getValue(), 0, 0, Example.FONT12, paint);
                canvas.translate(0, metrics.getHeight());
            }

            // FPS
            int len = (int) ((width - Example.PADDING * 2) / scale);
            if (len > 0 && times.length != len) {
                times = new double[len];
                timesIdx = 0;
            }

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
            canvas.drawString("FPS: " + fps, 0, 0, Example.FONT12, paint);
            canvas.restore();

            // VSync
            try (var line = TextLine.make("VSYNC", Example.FONT24); ) {
                var capHeight = Example.FONT24.getMetrics().getCapHeight();
                paint.setColor(0xFFE0E0E0);
                canvas.drawRRect(RRect.makeXYWH(width - line.getWidth() - 3 * Example.PADDING,
                                                Example.PADDING,
                                                line.getWidth() + 2 * Example.PADDING,
                                                capHeight + 2 * Example.PADDING,
                                                4 * scale), paint);
                
                paint.setColor(vsyncColor ? 0xFFEF8784 : 0xFFA1FCFE);
                canvas.drawTextLine(line, width - line.getWidth() - 2 * Example.PADDING, capHeight + 2 * Example.PADDING, paint);
                vsyncColor = !vsyncColor;
            }

            // FPS graph
            canvas.save();
            canvas.translate(Example.PADDING, height - Example.PADDING - 32 * scale);

            paint.setColor(0x4033cc33);
            canvas.drawRRect(RRect.makeXYWH(0, 0, width - Example.PADDING * 2, 32 * scale, 4 * scale, 4 * scale, 0, 0), paint);
            paint.setColor(0xFF33CC33);
            for (int i = 0; i < times.length; ++i) {
                var idx = (timesIdx + i) % times.length;
                canvas.drawRect(Rect.makeXYWH(i * scale, (32 - (float) times[idx]) * scale, 1 * scale, (float) (times[idx] * scale)), paint);
            }

            paint.setColor(0x20000000);
            canvas.drawRect(Rect.makeXYWH(0, (32 - 17) * scale, times.length * scale, 1 * scale), paint);
            canvas.drawRect(Rect.makeXYWH(0, (32 - 8) * scale, times.length * scale, 1 * scale), paint);
            canvas.restore();
        }
    }
}
