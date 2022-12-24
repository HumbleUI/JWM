package io.github.humbleui.jwm.examples;

import java.util.*;
import io.github.humbleui.jwm.*;
import io.github.humbleui.skija.*;
import io.github.humbleui.types.*;

public class PanelLegend extends Panel {
    public Map<String, String> shortcuts = new TreeMap<>();

    public PanelLegend(Window window) {
        super(window);
        shortcuts.put("L", "Toggle Layer");
        shortcuts.put("P", "Pause");
        shortcuts.put("N", "New Window");
        shortcuts.put("T", "Toggle Titlebar");
        shortcuts.put("W", "Close Window");
        shortcuts.put("F", "Toggle Fullscreen");
        shortcuts.put("X", "Clipboard formats");
        shortcuts.put("Y", "Hide mouse cursor");
        shortcuts.put("U", "Lock mouse cursor");
        shortcuts.put("Z", "Toggle Z-order");
        shortcuts.put("B", "Toggle Progress Bar");
        shortcuts.put("O", "Opacity");
        shortcuts.put("1", "Minimize");
        shortcuts.put("2", "Maximize");
        shortcuts.put("3", "Restore");
        shortcuts.put("4", "Hide");
        shortcuts.put("5", "Set position");
        shortcuts.put("6", "Set size");
    }

    @Override
    public void paintImpl(Canvas canvas, int width, int height, float scale) {
        var modifier = Platform.CURRENT == Platform.MACOS ? "⌘ " : "Ctrl ";
        var padding = (int) 8 * scale;

        try (var bg = new Paint().setColor(0x40000000);
             var fg = new Paint().setColor(0xFFFFFFFF);)
        {
            var metrics = Example.FONT12.getMetrics();
            var capHeight = metrics.getCapHeight();
            float bgWidth = 0;
            try (var line = TextLine.make(modifier + "W", Example.FONT12);) {
                bgWidth = line.getWidth() + 2 * padding;
            }
            float bgHeight = capHeight + padding * 2;
            float x = Example.PADDING;
            float y = Example.PADDING;

            for (var key: shortcuts.keySet()) {
                try (var line = TextLine.make(modifier + key, Example.FONT12);) {
                    canvas.drawRRect(RRect.makeXYWH(x, y, bgWidth, bgHeight, 4 * scale), bg);
                    canvas.drawTextLine(line, x + (bgWidth - line.getWidth()) / 2, y + padding + capHeight, fg);
                }

                var value = shortcuts.get(key);
                try (var line = TextLine.make(value, Example.FONT12);) {
                    canvas.drawTextLine(line, x + bgWidth + padding, y + padding + capHeight, fg);
                }
                
                y += padding * 2 + capHeight + 1 * scale;
            }
        }
    }
}
