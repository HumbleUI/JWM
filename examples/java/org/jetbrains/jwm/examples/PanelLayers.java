package org.jetbrains.jwm.examples;

import org.jetbrains.jwm.*;
import org.jetbrains.skija.*;

public class PanelLayers extends Panel {
    public String[] layers;
    public int layerIdx = 0;
    public SkijaLayer layer;

    public PanelLayers(Window window) {
        super(window);

        if (Platform.CURRENT == Platform.MACOS)
            layers = new String[] { "SkijaLayerGL", "macos.SkijaLayerMetal" };
        else if (Platform.CURRENT == Platform.WINDOWS)
            layers = new String[] { "SkijaLayerGL", "SkijaLayerRaster", "windows.SkijaLayerD3D12" };
        else
            layers = new String[] { "SkijaLayerGL", "SkijaLayerRaster" };
        
        changeLayer();
    }

    public void changeLayer() {
        if (layer != null) {
            layer.close();
            layer = null;
        }

        int attemptsCount = layers.length;

        while (layer == null && attemptsCount > 0) {
            String className = "org.jetbrains.jwm.examples." + layers[layerIdx];

            try {
                layer = (SkijaLayer) Example.class.forName(className).getDeclaredConstructor().newInstance();
                layer.attach(window);
            } catch (Exception e) {
                System.err.println("Failed to create class " + className);
                e.printStackTrace();
                layer = null;

                nextLayerIdx();
            }

            attemptsCount -= 1;
        }

        if (layer == null)
            throw new RuntimeException("No available layer to create");

        layer.reconfigure();
        layer.resize(window.getContentRect().getWidth(), window.getContentRect().getHeight());
    }

    @Override
    public void accept(Event e) {
        if (e instanceof EventWindowScreenChange) {
            layer.reconfigure();
            accept(new EventWindowResize(window.getWindowRect().getWidth(),
                                         window.getWindowRect().getHeight(),
                                         window.getContentRect().getWidth(),
                                         window.getContentRect().getHeight()));
        } else if (e instanceof EventWindowResize ee) {
            layer.resize(ee.getContentWidth(), ee.getContentHeight());
        } else if (e instanceof EventKey ee && ee.isPressed()) {
            Key key = ee.getKey();
            boolean modifier = ee.isModifierDown(Example.MODIFIER);
            if (Key.L == key && modifier) {
                nextLayerIdx();
                changeLayer();
            }
        } else if (e instanceof EventWindowCloseRequest) {
            layer.close();
        }
    }

    @Override
    public void paintImpl(Canvas canvas, int width, int height, float scale) {
        try (var paint = new Paint();) {
            paint.setColor(0xFFFFFFFF);
            var metrics = Example.FONT12.getMetrics();
            try (var shevron = TextLine.make("> ", Example.FONT12);) {
                canvas.save();
                canvas.translate(Example.PADDING, Example.PADDING - metrics.getAscent());
                for (int i = 0; i < layers.length; ++i) {
                    if (i == layerIdx)
                        canvas.drawTextLine(shevron, 0, 0, paint);
                    canvas.drawString(layers[i], shevron.getWidth(), 0, Example.FONT12, paint);
                    canvas.translate(0, metrics.getHeight());
                }
                canvas.restore();
            }

            try (var shortcut = TextLine.make(Platform.CURRENT == Platform.MACOS ? "⌘ L" : "Ctrl L", Example.FONT12);) {
                var padding = (int) 8 * scale;
                var capHeight = metrics.getCapHeight();
                paint.setColor(0x40000000);
                canvas.drawRRect(RRect.makeXYWH(Example.PADDING,
                                                height - Example.PADDING - padding * 2 - capHeight,
                                                shortcut.getWidth() + padding * 2,
                                                capHeight + padding * 2, 4 * scale), paint);
                paint.setColor(0xFFFFFFFF);
                canvas.drawTextLine(shortcut,
                                    Example.PADDING + padding,
                                    height - Example.PADDING - padding,
                                    paint);
                canvas.drawString("Toggle Layer",
                                  Example.PADDING + padding * 3 + shortcut.getWidth(),
                                  height - Example.PADDING - padding,
                                  Example.FONT12,
                                  paint);
            }
        }
    }

    private int nextLayerIdx() {
        layerIdx = (layerIdx + 1) % layers.length;
        return layerIdx;
    }
}