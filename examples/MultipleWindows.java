package org.jetbrains.jwm.examples;

import java.util.function.*;
import java.util.*;
import lombok.*;
import org.jetbrains.annotations.*;
import org.jetbrains.jwm.*;
import org.jetbrains.jwm.macos.*;
import org.jetbrains.skija.*;

class SkiaWindow {
    public final Window window;
    public final ContextMetal contextMtl;
    public final DirectContext directContext;
    public int angle = 0;

    public SkiaWindow(Window window, ContextMetal contextMtl, DirectContext directContext) {
        this.window = window;
        this.contextMtl = contextMtl;
        this.directContext = directContext;
    }

    public void paint() {
        long texturePtr = contextMtl.nextDrawableTexturePtr();
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

            try (var paint = new Paint()) {
                int[] colors = new int[] { 0xFFe76f51, 0xFF2a9d8f, 0xFFe9c46a };
                canvas.drawTriangles(new Point[] { new Point(20, 20), new Point(400, 20), new Point (20, 400) }, colors, paint);
                canvas.drawTriangles(new Point[] { new Point(width - 20, 20), new Point(width - 400, 20), new Point(width - 20, 400) }, colors, paint);
                canvas.drawTriangles(new Point[] { new Point(20, height - 20), new Point(400, height - 20), new Point (20, height - 400) }, colors, paint);
                canvas.drawTriangles(new Point[] { new Point(width - 20, height - 20), new Point(width - 400, height - 20), new Point(width - 20, height - 400) }, colors, paint);

                canvas.save();
                canvas.translate(width / 2, height / 2);
                paint.setColor(0xFFFFFFFF);
                canvas.drawCircle(0, 0, 200, paint);
                angle = (angle + 3) % 360;
                canvas.rotate(angle);
                paint.setColor(0xFF264653);
                canvas.drawRect(Rect.makeXYWH(-15, -200, 30, 400), paint);
                canvas.restore();
            }

            surface.flushAndSubmit();
            contextMtl.swapBuffers();
        }
    }

    public static SkiaWindow create() {
        final Window window = App.makeWindow();
        final ContextMetal contextMtl = new ContextMetal(ContextMetalOpts.DEFAULT);
        window.attach(contextMtl);
        final DirectContext directContext = DirectContext.makeMetal(contextMtl.getDevicePtr(), contextMtl.getQueuePtr());
        final SkiaWindow skiaWindow = new SkiaWindow(window, contextMtl, directContext);

        window.setEventListener(e -> {
            if (e instanceof EventClose) {
                window.close();
            } else if (e instanceof EventKeyboard) {
                EventKeyboard eventKeyboard = (EventKeyboard) e;
                if (eventKeyboard.getKeyCode() == 53 // Esc
                    && eventKeyboard.isPressed() == true)
                {
                    window.close();
                }
            } else if (e instanceof EventPaint)
                skiaWindow.paint();
        });
        return skiaWindow;
    }
}

public class MultipleWindows {
    public Set<SkiaWindow> windows = new HashSet<SkiaWindow>();

    public void run() {
        App.init();
        for (int i = 0; i < 2; i++) {
            SkiaWindow window = SkiaWindow.create();
            windows.add(window);
            window.window.show();
        }
        App.runEventLoop();
    }

    public static void main(String[] args) {
        new MultipleWindows().run();
    }
}
