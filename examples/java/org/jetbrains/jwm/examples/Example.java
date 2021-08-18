package org.jetbrains.jwm.examples;

import org.jetbrains.jwm.*;
import org.jetbrains.skija.*;

import java.util.*;
import java.util.concurrent.*;
import java.util.function.*;
import java.util.stream.*;

public class Example implements Consumer<Event>, TextInputClient {
    public Window window;
    public SkijaLayer layer;
    public static Timer timer = new Timer(true);
    public TimerTask timerTask;

    public int angle = 0;
    public Font font = new Font(FontMgr.getDefault().matchFamilyStyleCharacter(null, FontStyle.NORMAL, null, "↑".codePointAt(0)), 12);
    public Font font24 = new Font(FontMgr.getDefault().matchFamilyStyle(null, FontStyle.NORMAL), 24);
    public Font font48 = new Font(FontMgr.getDefault().matchFamilyStyle(null, FontStyle.BOLD), 48);
    public EventMouseMove lastMouseMove = null;
    public EventWindowResize lastResize = new EventWindowResize(0, 0, 0, 0);
    public EventWindowMove lastMove = new EventWindowMove(0, 0);
    public Point scroll = new Point(0, 0);
    public String text = "";
    public EventTextInputMarked lastMarked = null;
    public Map<String, Integer> paintCounters = new ConcurrentHashMap<>();

    public String[] variants;
    public int variantIdx = 0;

    public long t0 = System.nanoTime();
    public double[] times = new double[180];
    public int timesIdx = 0;
    public boolean paused = false;

    public Set<Key> keys = Collections.synchronizedSortedSet(new TreeSet<Key>());
    public Set<MouseButton> buttons = Collections.synchronizedSortedSet(new TreeSet<MouseButton>());

    public Example() {
        if (Platform.CURRENT == Platform.MACOS)
            variants = new String[] { "SkijaLayerGL", "macos.SkijaLayerMetal" };
        else if (Platform.CURRENT == Platform.WINDOWS)
            variants = new String[] { "SkijaLayerGL", "SkijaLayerRaster", "windows.SkijaLayerD3D12" };
        else
            variants = new String[] { "SkijaLayerGL", "SkijaLayerRaster" };

        window = App.makeWindow();
        window.setEventListener(this);
        window.setTextInputClient(this);
        window.setTextInputEnabled(true);
        changeLayer();
        var scale = window.getScreen().getScale();
        window.setWindowPosition((int) (100 * scale), (int) (100 * scale));
        window.setWindowSize((int) (800 * scale), (int) (600 * scale));
        window.show();
        window.requestFrame();

        timerTask = new TimerTask() {
            public void run() {
                App.runOnUIThread(() -> { paint("Timer"); });
            }
        };
        timer.schedule(timerTask, 10, 1000);
    }

    public void paint(String reason) {
        paintCounters.merge(reason, 1, Integer::sum);

        if (layer == null)
            return;

        var canvas = layer.beforePaint();

        canvas.clear(0xFF264653);
        int canvasCount = canvas.save();
        float scale = window.getScreen().getScale();
        int width = (int) (window.getContentRect().getWidth() / scale);
        int height = (int) (window.getContentRect().getHeight() / scale);
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
                var x = (int) (lastMouseMove.getX() / window.getScreen().getScale());
                var y = (int) (lastMouseMove.getY() / window.getScreen().getScale());
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

        // Text
        try (var paint = new Paint()) {
            canvas.save();
            var lines = text.split("\n", -1);
            paint.setColor(0xFFFFFFFF);
            canvas.translate((width - 300) / 2, 20);
            canvas.drawRRect(RRect.makeXYWH(0, 0, 300, 50, 4), paint);
            canvas.clipRect(Rect.makeXYWH(1, 1, 298, 48));
            paint.setColor(0xFF000000);
            var metrics = font24.getMetrics();
            var lineHeight = metrics.getHeight();
            var baseline = 50 - (50 - lineHeight) / 2 - metrics.getDescent();
            if (lines.length > 1) {
                paint.setColor(0xFF000000);
                try (var line = TextLine.make(lines[lines.length - 2], font24)) {
                    canvas.drawTextLine(line, 8, baseline - lineHeight, paint);
                }
            }
            
            try (var line = TextLine.make(lines[lines.length - 1], font24)) {
                canvas.drawTextLine(line, 8, baseline, paint);
                canvas.translate(8 + line.getWidth(), 0);

                // marked text
                paint.setColor(0xFF0087D8);
                if (lastMarked != null) {
                    try (var marked = TextLine.make(lastMarked.getText(), font24)) {
                        canvas.drawTextLine(marked, 0, baseline, paint);
                        var start = marked.getCoordAtOffset(lastMarked.getSelectionStart());
                        var end = marked.getCoordAtOffset(lastMarked.getSelectionEnd());
                        if (0 < start - 1)
                            canvas.drawRect(Rect.makeLTRB(0, baseline + 2, start - 1, baseline + 4), paint);
                        if (start + 1 < end - 1)
                            canvas.drawRect(Rect.makeLTRB(start + 1, baseline + 2, end - 1, baseline + 6), paint);
                        if (end + 1 < marked.getWidth())
                            canvas.drawRect(Rect.makeLTRB(end + 1, baseline + 2, marked.getWidth(), baseline + 4), paint);

                        canvas.translate(marked.getWidth(), 0);
                    }
                }

                // cursor                
                canvas.drawRect(Rect.makeXYWH(0, baseline + metrics.getAscent() - 2, 2, metrics.getHeight() + 4), paint);
            }
            canvas.restore();
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
        var hudHeight = 8 * 2 + 32 + 24 * (11 + paintCounters.size());
        try (var paint = new Paint()) {
            canvas.save();
            canvas.translate(width - (8 + 180 + 8 + 8), height - 8 - hudHeight - 68);
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
            canvas.translate(width - (8 + 180 + 8 + 8), height - 8 - hudHeight);

            // bg
            paint.setColor(0x40000000);
            canvas.drawRRect(RRect.makeXYWH(0, 0, 8 + 180 + 8, hudHeight, 4), paint);
            canvas.translate(8, 8);

            // Dimensions
            paint.setColor(0xFFFFFFFF);
            canvas.drawString("Window: " + window.getWindowRect(), 0, 12, font, paint);
            canvas.translate(0, 24);
            canvas.drawString("Content: " + window.getContentRect(), 0, 12, font, paint);
            canvas.translate(0, 24);
            canvas.drawString("Resize: " + lastResize.getWindowWidth() + "×" + lastResize.getWindowHeight() + " " + lastResize.getContentWidth() + "×" + lastResize.getContentHeight(), 0, 12, font, paint);
            canvas.translate(0, 24);
            canvas.drawString("Move: (" + lastMove.getWindowLeft() + "," + lastMove.getWindowTop() + ")", 0, 12, font, paint);
            canvas.translate(0, 24);
            canvas.drawString("Scale: " + window.getScreen().getScale(), 0, 12, font, paint);
            canvas.translate(0, 24);

            // Variant
            paint.setColor(0x80000000);
            canvas.drawRRect(RRect.makeXYWH(0, 0, 16, 16, 2), paint);
            canvas.drawRRect(RRect.makeXYWH(20, 0, 16, 16, 2), paint);
            canvas.drawRRect(RRect.makeXYWH(40, 0, 16, 16, 2), paint);
            paint.setColor(0xFFFFFFFF);
            canvas.drawString("1", 3, 12, font, paint);
            canvas.drawString("2", 23, 12, font, paint);
            canvas.drawString("3", 43, 12, font, paint);
            canvas.drawString("Pos / Wndw / Cont", 64, 12, font, paint);
            canvas.translate(0, 24);

            paint.setColor(0x80000000);
            canvas.drawRRect(RRect.makeXYWH(0, 0, 16, 16, 2), paint);
            paint.setColor(0xFFFFFFFF);
            canvas.drawString("L", 3, 12, font, paint);
            canvas.drawString(variants[variantIdx], 24, 12, font, paint);
            canvas.translate(0, 24);

            paint.setColor(0x80000000);
            canvas.drawRRect(RRect.makeXYWH(0, 0, 16, 16, 2), paint);
            paint.setColor(0xFFFFFFFF);
            canvas.drawString("P", 3, 12, font, paint);
            canvas.drawString(paused ? "Paused" : "Not paused", 24, 12, font, paint);
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

            // Paint counters
            for (var entry: paintCounters.entrySet()) {
                paint.setColor(0xFFFFFFFF);
                canvas.drawString(entry.getKey() + ": " + entry.getValue(), 0, 12, font, paint);
                canvas.translate(0, 24);
            }

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

        canvas.restoreToCount(canvasCount);

        layer.afterPaint();
    }

    public void changeLayer() {
        if (layer != null)
            layer.close();

        String className = "org.jetbrains.jwm.examples." + variants[variantIdx];

        try {
            layer = (SkijaLayer) Example.class.forName(className).getDeclaredConstructor().newInstance();
        } catch (Exception e) {
            System.err.println("Failed to create class " + className);
            e.printStackTrace();
            layer = null;
        }

        layer.attach(window);
        layer.reconfigure();
        layer.resize(window.getContentRect().getWidth(), window.getContentRect().getHeight());
    }

    @Override
    public void accept(Event e) {
        float scale = window.getScreen().getScale();
        if (e instanceof EventEnvironmentChange) {
            layer.reconfigure();
            accept(new EventWindowResize(window.getWindowRect().getWidth(),
                                         window.getWindowRect().getHeight(),
                                         window.getContentRect().getWidth(),
                                         window.getContentRect().getHeight()));
        } else if (e instanceof EventWindowResize ee) {
            lastResize = ee;
            layer.resize(ee.getContentWidth(), ee.getContentHeight());
            paint("Resize");
        } else if (e instanceof EventTextInput ee) {
            text += ee.getText();
            lastMarked = null;
        } else if (e instanceof EventTextInputMarked ee) {
            System.out.println(ee);
            lastMarked = ee;
        } else if (e instanceof EventMouseButton ee) {
            window.unmarkText();
            if (ee.isPressed())
                buttons.add(ee.getButton());
            else
                buttons.remove(ee.getButton());
        } else if (e instanceof EventMouseMove ee) {
            lastMouseMove = ee;
        } else if (e instanceof EventKey eventKey) {
            KeyModifier modifier = Platform.CURRENT == Platform.MACOS ? KeyModifier.COMMAND : KeyModifier.CONTROL;
            if (eventKey.isPressed() == true && eventKey.isModifierDown(modifier)) {
                switch (eventKey.getKey()) {
                    case DIGIT1 ->
                        window.setWindowPosition((int) (200 * scale), (int) (200 * scale));
                    case DIGIT2 ->
                        window.setWindowSize((int) (600 * scale), (int) (500 * scale));
                    case DIGIT3 ->
                        window.setContentSize((int) (600 * scale), (int) (500 * scale));
                    case P -> {
                        paused = !paused;
                        if (!paused)
                            window.requestFrame();
                    }
                    case N ->
                        new Example();
                    case W ->
                        accept(EventWindowCloseRequest.INSTANCE);
                    case C ->
                        Clipboard.set(ClipboardEntry.makePlainText(text));
                    case V -> {
                        ClipboardEntry entry = Clipboard.get(ClipboardFormat.TEXT);
                        if (entry != null)
                            text = entry.getString();
                    }
                    case F -> {
                        ClipboardFormat[] formats = Clipboard.getFormats();
                        if (formats != null)
                            for (ClipboardFormat format: formats)
                                System.out.println(format.getFormatId());
                    }
                    case L -> {
                        variantIdx = (variantIdx + 1) % variants.length;
                        changeLayer();
                    }
                }
            }

            if (eventKey.isPressed() == true) {
                switch(eventKey.getKey()) {
                    case ENTER -> {
                        text += "\n";
                        break;
                    }
                    case BACK_SPACE -> {
                        if (lastMarked == null && text.length() > 0) {
                            try (var iter = BreakIterator.makeCharacterInstance();) {
                                iter.setText(text);
                                text = text.substring(0, iter.preceding(text.length()));
                            }
                        }

                    }
                }
            }

            if (eventKey.isPressed() == true)
                keys.add(eventKey.getKey());
            else
                keys.remove(eventKey.getKey());
        } else if (e instanceof EventMouseScroll ee) {
            scroll = scroll.offset(ee.getDeltaX(), ee.getDeltaY());
        } else if (e instanceof EventWindowMove ee) {
            lastMove = ee;
        } else if (e instanceof EventFrame) {
            paint("Frame");
            if (!paused)
                window.requestFrame();
        } else if (e instanceof EventWindowCloseRequest) {
            timerTask.cancel();
            layer.close();
            window.close();
            if (App._windows.size() == 0)
                App.terminate();
        }
    }

    @Override
    public UIRect getRectForMarkedRange(int selectionStart, int selectionEnd) {
        System.out.println("TextInputClient::getRectForMarkedRange " + selectionStart + ".." + selectionEnd);

        float scale = window.getScreen().getScale();
        var lines = text.split("\n", -1);
        try (var line = TextLine.make(lines[lines.length - 1], font24);) {
            var left = (window.getContentRect().getWidth() / scale - 300) / 2 + 8 + line.getWidth();
            var metrics = font24.getMetrics();
            var top = 50 - (50 - metrics.getHeight()) / 2 + 20 - metrics.getHeight();

            if (lastMarked != null) {
                try (var marked = TextLine.make(lastMarked.getText(), font24)) {
                    var start = marked.getCoordAtOffset(selectionStart);
                    var end = marked.getCoordAtOffset(selectionEnd);
                    return UIRect.makeXYWH((int) (scale * (left + start)),
                                           (int) (scale * top),
                                           (int) (scale * (end - start)),
                                           (int) (scale * metrics.getHeight()));
                }
            } else {
                return UIRect.makeXYWH((int) (scale * left),
                                       (int) (scale * top),
                                       0,
                                       (int) (scale * metrics.getHeight()));
            }
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
