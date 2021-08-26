package org.jetbrains.jwm.examples;

import org.jetbrains.jwm.*;
import org.jetbrains.skija.*;

public class PanelLegend extends Panel {
    public PanelLegend(Window window) {
        super(window);
    }

    @Override
    public void paintImpl(Canvas canvas, int width, int height, float scale) {
        var modifier = Platform.CURRENT == Platform.MACOS ? "⌘ " : "Ctrl ";
        var padding = (int) 8 * scale;
        try (var bg = new Paint().setColor(0x40000000);
             var fg = new Paint().setColor(0xFFFFFFFF);
             var pause = TextLine.make(modifier + "P", Example.FONT12);
             var open = TextLine.make(modifier + "N", Example.FONT12);
             var close = TextLine.make(modifier + "W", Example.FONT12);
             var formats = TextLine.make(modifier + "F", Example.FONT12);)
        {
            var metrics = Example.FONT12.getMetrics();
            var capHeight = metrics.getCapHeight();
            var col2 = close.getWidth() + 3 * padding;

            canvas.save();
            canvas.translate(Example.PADDING, Example.PADDING);
            
            canvas.drawRRect(RRect.makeXYWH(0, 0, padding * 2 + pause.getWidth(), padding * 2 + capHeight, 4 * scale), bg);
            canvas.drawTextLine(pause, padding, padding + capHeight, fg);
            canvas.drawString("Pause", col2, padding + capHeight, Example.FONT12, fg);
            canvas.translate(0, padding * 3 + capHeight);

            canvas.drawRRect(RRect.makeXYWH(0, 0, padding * 2 + open.getWidth(), padding * 2 + capHeight, 4 * scale), bg);
            canvas.drawTextLine(open, padding, padding + capHeight, fg);
            canvas.drawString("New Window", col2, padding + capHeight, Example.FONT12, fg);
            canvas.translate(0, padding * 3 + capHeight);

            canvas.drawRRect(RRect.makeXYWH(0, 0, padding * 2 + close.getWidth(), padding * 2 + capHeight, 4 * scale), bg);
            canvas.drawTextLine(close, padding, padding + capHeight, fg);
            canvas.drawString("Close Window", col2, padding + capHeight, Example.FONT12, fg);
            canvas.translate(0, padding * 3 + capHeight);

            canvas.drawRRect(RRect.makeXYWH(0, 0, padding * 2 + formats.getWidth(), padding * 2 + capHeight, 4 * scale), bg);
            canvas.drawTextLine(formats, padding, padding + capHeight, fg);
            canvas.drawString("Clipboard formats", col2, padding + capHeight, Example.FONT12, fg);
            canvas.translate(0, padding * 3 + capHeight);
       }
    }
}