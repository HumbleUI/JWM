package org.jetbrains.jwm.examples;

import org.jetbrains.jwm.*;
import org.jetbrains.skija.*;

import java.util.*;
import java.util.concurrent.*;
import java.util.function.*;
import java.util.stream.*;
import java.io.File;


public class Example implements Consumer<Event> {
    public static int PADDING = 10;
    public static int ROWS = 3, COLS = 4;
    public static final KeyModifier MODIFIER = Platform.CURRENT == Platform.MACOS ? KeyModifier.COMMAND : KeyModifier.CONTROL;
    public static Font FONT12 = new Font(FontMgr.getDefault().matchFamilyStyleCharacter(null, FontStyle.NORMAL, null, "↑".codePointAt(0)), 12);
    public static Font FONT24 = new Font(FontMgr.getDefault().matchFamilyStyle(null, FontStyle.NORMAL), 24);
    public static Font FONT48 = new Font(FontMgr.getDefault().matchFamilyStyle(null, FontStyle.BOLD), 48);

    public float lastScale = 1f;
    public PanelTextInput panelTextInput;
    public PanelVSync panelVSync;
    public PanelAnimation panelAnimation;
    public PanelKeys panelKeys;
    public PanelMouseButtons panelMouseButtons;
    public PanelMouseScroll panelMouseScroll;
    public PanelMousePosition panelMousePosition;
    public PanelFrames panelFrames;
    public PanelLayers panelLayers;
    public PanelScreens panelScreens;
    public PanelLegend panelLegend;

    public Window window;
    public static Timer timer = new Timer(true);
    public TimerTask timerTask;

    public boolean paused = false;
    public boolean closed = false;

    public Example() {
        window = App.makeWindow();
        window.setEventListener(this);

        panelTextInput = new PanelTextInput(window);
        panelVSync = new PanelVSync();
        panelAnimation = new PanelAnimation();
        panelKeys = new PanelKeys();
        panelMouseButtons = new PanelMouseButtons();
        panelMouseScroll = new PanelMouseScroll();
        panelMousePosition = new PanelMousePosition();
        panelFrames = new PanelFrames();
        panelLayers = new PanelLayers(window);
        panelScreens = new PanelScreens(window);
        panelLegend = new PanelLegend();

        var scale = window.getScreen().getScale();
        int count = App._windows.size() - 1;
        Screen screen = App.getScreens()[(count / 5) % App.getScreens().length];
        UIRect bounds = screen.getWorkArea();

        window.setWindowSize(bounds.getWidth() / 2, bounds.getHeight() / 2);
        switch (count % 5) {
            case 0 -> window.setWindowPosition(bounds.getLeft() + bounds.getWidth() / 4, bounds.getTop() + bounds.getHeight() / 4);
            case 1 -> window.setWindowPosition(bounds.getLeft(), bounds.getTop());
            case 2 -> window.setWindowPosition(bounds.getLeft() + bounds.getWidth() / 2, bounds.getTop());
            case 3 -> window.setWindowPosition(bounds.getLeft(), bounds.getTop() + bounds.getHeight() / 2);
            case 4 -> window.setWindowPosition(bounds.getLeft() + bounds.getWidth() / 2, bounds.getTop() + bounds.getHeight() / 2);
        }
        window.setTitle("JWM Window #" + count);
        window.setIcon(new File("sample.ico"));
        window.setMouseCursor(MouseCursor.ARROW);
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
        if (closed)
            return;

        float scale = window.getScreen().getScale();
        PADDING = (int) (10 * scale);
        int panelWidth = (window.getContentRect().getWidth() - (COLS + 1) * PADDING) / COLS;
        int panelHeight = (window.getContentRect().getHeight() - (ROWS + 1) * PADDING) / ROWS;

        if (lastScale != scale) {
            FONT12.setSize(12 * scale);
            FONT24.setSize(24 * scale);
            FONT48.setSize(48 * scale);
        }

        var canvas = panelLayers.layer.beforePaint();
        canvas.clear(0xFF264653);
        int canvasCount = canvas.save();

        // First row
        panelKeys.paint         (canvas, PADDING + (panelWidth + PADDING) * 0, PADDING + (panelHeight + PADDING) * 0, panelWidth, panelHeight, scale);
        panelTextInput.paint    (canvas, PADDING + (panelWidth + PADDING) * 1, PADDING + (panelHeight + PADDING) * 0, panelWidth * 2 + PADDING, panelHeight, scale);
        panelScreens.paint      (canvas, PADDING + (panelWidth + PADDING) * 3, PADDING + (panelHeight + PADDING) * 0, panelWidth, panelHeight, scale);
        
        // Second row
        panelMouseButtons.paint (canvas, PADDING + (panelWidth + PADDING) * 0, PADDING + (panelHeight + PADDING) * 1, panelWidth, panelHeight, scale);
        panelMouseScroll.paint  (canvas, PADDING + (panelWidth + PADDING) * 1, PADDING + (panelHeight + PADDING) * 1, panelWidth, panelHeight, scale);
        panelMousePosition.paint(canvas, PADDING + (panelWidth + PADDING) * 2, PADDING + (panelHeight + PADDING) * 1, panelWidth, panelHeight, scale);
        panelLayers.paint       (canvas, PADDING + (panelWidth + PADDING) * 3, PADDING + (panelHeight + PADDING) * 1, panelWidth, panelHeight, scale);
        
        // Third row
        panelVSync.paint        (canvas, PADDING + (panelWidth + PADDING) * 0, PADDING + (panelHeight + PADDING) * 2, panelWidth, panelHeight, scale);
        panelAnimation.paint    (canvas, PADDING + (panelWidth + PADDING) * 1, PADDING + (panelHeight + PADDING) * 2, panelWidth, panelHeight, scale);
        panelFrames.bumpCounter(reason);
        panelFrames.paint       (canvas, PADDING + (panelWidth + PADDING) * 2, PADDING + (panelHeight + PADDING) * 2, panelWidth, panelHeight, scale);
        panelLegend.paint       (canvas, PADDING + (panelWidth + PADDING) * 3, PADDING + (panelHeight + PADDING) * 2, panelWidth, panelHeight, scale);

        // Colored bars
        try (var paint = new Paint()) {
            int width = window.getContentRect().getWidth();
            int height = window.getContentRect().getHeight();
            var barSize = 3 * scale;

            // left
            paint.setColor(0xFFe76f51);
            canvas.drawRect(Rect.makeXYWH(0, 0, barSize, 100 * scale), paint);
            canvas.drawRect(Rect.makeXYWH(0, height / 2 - 50 * scale, barSize, 100 * scale), paint);
            canvas.drawRect(Rect.makeXYWH(0, height - 100 * scale, barSize, 100 * scale), paint);

            // top
            paint.setColor(0xFF2a9d8f);
            canvas.drawRect(Rect.makeXYWH(0, 0, 100 * scale, barSize), paint);
            canvas.drawRect(Rect.makeXYWH(width / 2 - 50 * scale, 0, 100 * scale, barSize), paint);
            canvas.drawRect(Rect.makeXYWH(width - 100 * scale, 0, 100 * scale, barSize), paint);

            // right
            paint.setColor(0xFFe9c46a);
            canvas.drawRect(Rect.makeXYWH(width - barSize, 0, barSize, 100 * scale), paint);
            canvas.drawRect(Rect.makeXYWH(width - barSize, height / 2 - 50 * scale, barSize, 100 * scale), paint);
            canvas.drawRect(Rect.makeXYWH(width - barSize, height - 100 * scale, barSize, 100 * scale), paint);

            // bottom
            paint.setColor(0xFFFFFFFF);
            canvas.drawRect(Rect.makeXYWH(0, height - barSize, 100 * scale, barSize), paint);
            canvas.drawRect(Rect.makeXYWH(width / 2 - 50 * scale, height - barSize, 100 * scale, barSize), paint);
            canvas.drawRect(Rect.makeXYWH(width - 100 * scale, height - barSize, 100 * scale, barSize), paint);
        }
        canvas.restoreToCount(canvasCount);

        panelLayers.layer.afterPaint();
    }

    @Override
    public void accept(Event e) {
        panelTextInput.accept(e);
        panelKeys.accept(e);
        panelMouseButtons.accept(e);
        panelMouseScroll.accept(e);
        panelMousePosition.accept(e);
        panelLayers.accept(e);
        panelScreens.accept(e);

        float scale = window.getScreen().getScale();
        if (e instanceof EventKey eventKey) {
            if (eventKey.isPressed() == true && eventKey.isModifierDown(MODIFIER)) {
                switch (eventKey.getKey()) {
                    case P -> {
                        paused = !paused;
                        if (!paused)
                            window.requestFrame();
                    }
                    case N ->
                        new Example();
                    case W ->
                        accept(EventWindowCloseRequest.INSTANCE);
                    case UP ->
                        window.maximize();
                    case DOWN ->
                        window.restore();
                    case M ->
                        window.minimize();
                }
            }
        } else if (e instanceof EventWindowResize ee) {
            paint("Resize");
        } else if (e instanceof EventFrame) {
            paint("Frame");
            if (!paused)
                window.requestFrame();
        } else if (e instanceof EventWindowCloseRequest) {
            closed = true;
            timerTask.cancel();
            window.close();
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
