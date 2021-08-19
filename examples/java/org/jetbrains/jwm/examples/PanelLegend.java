package org.jetbrains.jwm.examples;

import org.jetbrains.jwm.*;
import org.jetbrains.skija.*;

public class PanelLegend extends Panel {
    @Override
    public void paintImpl(Canvas canvas, int width, int height, float scale) {
        var modifier = Platform.CURRENT == Platform.MACOS ? "⌘ " : "Ctrl ";
        var padding = (int) 5 * scale;
        try (var bg = new Paint().setColor(0x40000000);
             var fg = new Paint().setColor(0xFFFFFFFF);
             var pause = TextLine.make(modifier + "P", Example.FONT12);
             var open = TextLine.make(modifier + "N", Example.FONT12);
             var close = TextLine.make(modifier + "W", Example.FONT12);)
        {
            var metrics = Example.FONT12.getMetrics();

            canvas.save();
            canvas.translate(Example.PADDING, Example.PADDING);
            
            canvas.drawRRect(RRect.makeXYWH(0, 0, padding * 2 + pause.getWidth(), padding * 2 + pause.getHeight(), 4 * scale), bg);
            canvas.drawTextLine(pause, padding, padding - metrics.getAscent(), fg);
            canvas.drawString("Pause", padding * 2 + Example.PADDING + pause.getWidth(), padding - metrics.getAscent(), Example.FONT12, fg);
            canvas.translate(0, padding * 2 + Example.PADDING + pause.getHeight());

            canvas.drawRRect(RRect.makeXYWH(0, 0, padding * 2 + open.getWidth(), padding * 2 + open.getHeight(), 4 * scale), bg);
            canvas.drawTextLine(open, padding, padding - metrics.getAscent(), fg);
            canvas.drawString("New Window", padding * 2 + Example.PADDING + open.getWidth(), padding - metrics.getAscent(), Example.FONT12, fg);
            canvas.translate(0, padding * 2 + Example.PADDING + open.getHeight());

            canvas.drawRRect(RRect.makeXYWH(0, 0, padding * 2 + close.getWidth(), padding * 2 + close.getHeight(), 4 * scale), bg);
            canvas.drawTextLine(close, padding, padding - metrics.getAscent(), fg);
            canvas.drawString("Close Window", padding * 2 + Example.PADDING + close.getWidth(), padding - metrics.getAscent(), Example.FONT12, fg);
            canvas.translate(0, padding * 2 + Example.PADDING + close.getHeight());
       }
    }
}