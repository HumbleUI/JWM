package org.jetbrains.jwm.examples;

import java.util.*;
import java.util.function.*;
import lombok.*;
import org.jetbrains.annotations.*;
import org.jetbrains.jwm.*;
import org.jetbrains.jwm.macos.*;
import org.jetbrains.skija.*;

public class SingleWindow {
    public Window window;
    public ContextMetal contextMtl;
    public DirectContext directContext;
    public int angle = 0;
    public Font font = new Font(FontMgr.getDefault().matchFamilyStyleCharacter(null, FontStyle.NORMAL, null, "↑".codePointAt(0)), 12);

    public String[] variants = new String[] {
        "Metal +vsync +transact", 
        "Metal +vsync -transact",
        "Metal -vsync +transact",
        "Metal -vsync -transact",
    };
    public int variantIdx = 0;

    public long t0 = System.nanoTime();
    public double[] times = new double[180];
    public int timesIdx = 0;

    public void paint() {
        long texturePtr = contextMtl.nextDrawableTexturePtr();
        float scale = 2; // TODO
        int width = contextMtl.getWidth();
        int height = contextMtl.getHeight();

        try (var renderTarget = BackendRenderTarget.makeMetal(width, height, texturePtr);
             var surface = Surface.makeFromBackendRenderTarget(
                             directContext,
                             renderTarget,
                             SurfaceOrigin.TOP_LEFT,
                             SurfaceColorFormat.BGRA_8888,
                             ColorSpace.getSRGB(),  // TODO load monitor profile
                             new SurfaceProps(PixelGeometry.RGB_H)))
        {
            var canvas = surface.getCanvas();
            canvas.clear(0xFF264653);
            int layer = canvas.save();
            canvas.scale(scale, scale);
            width = (int) (width / scale);
            height = (int) (height / scale);

            try (var paint = new Paint()) {
                int[] colors = new int[] { 0xFFe76f51, 0xFF2a9d8f, 0xFFe9c46a };
                canvas.drawTriangles(new Point[] { new Point(10, 10), new Point(200, 10), new Point (10, 200) }, colors, paint);
                canvas.drawTriangles(new Point[] { new Point(width - 10, 10), new Point(width - 200, 10), new Point(width - 10, 200) }, colors, paint);
                canvas.drawTriangles(new Point[] { new Point(10, height - 10), new Point(200, height - 10), new Point (10, height - 200) }, colors, paint);
                // canvas.drawTriangles(new Point[] { new Point(width - 10, height - 10), new Point(width - 200, height - 10), new Point(width - 10, height - 200) }, colors, paint);

                canvas.save();
                canvas.translate(width / 2, height / 2);
                paint.setColor(0xFFFFFFFF);
                canvas.drawCircle(0, 0, 100, paint);
                angle = (angle + 3) % 360;
                canvas.rotate(angle);
                paint.setColor(0xFF264653);
                canvas.drawRect(Rect.makeXYWH(-7, -100, 14, 200), paint);
                canvas.restore();
            }

            try (var paint = new Paint();)
            {
                canvas.save();
                canvas.translate(width - (8 + 180 + 8 + 8), height - (8 + 24 + 24 + 32 + 8 + 8));

                // bg
                paint.setColor(0x80000000);
                canvas.drawRRect(RRect.makeXYWH(0, 0, 8 + 180 + 8, 8 + 24 + 24 + 32 + 8, 4), paint);
                canvas.translate(8, 8);

                // Variant
                paint.setColor(0xFFFFFFFF);
                canvas.drawString("↓↑ " + variants[variantIdx], 0, 12, font, paint);
                canvas.translate(0, 24);

                int frames = 0;
                double time = 0;
                for (int i = 0; i < times.length; ++i) {
                    var idx = (timesIdx - i + times.length) % times.length;
                    time += times[idx];
                    frames++;
                    if (time > 1000)
                        break;
                }
                canvas.drawString("FPS: " + String.format("%.01f", (frames / time * 1000)), 0, 12, font, paint);
                canvas.translate(0, 24);

                // FPS
                long t1 = System.nanoTime();
                times[timesIdx] = (t1 - t0) / 1000000.0;
                t0 = t1;
                timesIdx = (timesIdx + 1) % times.length;
                
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

            surface.flushAndSubmit();
            contextMtl.swapBuffers();
        }
    }

    public void run() {
        App.init();
        window = App.makeWindow();
        contextMtl = new ContextMetal(true);
        window.attach(contextMtl);
        directContext = DirectContext.makeMetal(contextMtl.getDevicePtr(), contextMtl.getQueuePtr());
        window.setEventListener(e -> {
            if (e instanceof EventClose) {
                window.close();
                App.terminate();
            } else if (e instanceof EventKeyboard) {
                EventKeyboard eventKeyboard = (EventKeyboard) e;
                if (eventKeyboard.isPressed() == true) {
                    if (eventKeyboard.getKeyCode() == 53) { // Esc
                        window.close();
                        App.terminate();
                    } else if (eventKeyboard.getKeyCode() == 125) { // ↓
                        variantIdx = (variantIdx + 1) % variants.length;
                    } else if (eventKeyboard.getKeyCode() == 126) { // ↑
                        variantIdx = (variantIdx + variants.length - 1) % variants.length;
                    } else 
                        System.out.println("Key pressed: " + eventKeyboard.getKeyCode());
                }
            } else if (e instanceof EventPaint) {
                paint();
            }
        });
        window.show();
        App.runEventLoop();
    }

    public static void main(String[] args) {
        new SingleWindow().run();
    }
}
