package org.jetbrains.jwm.examples;

import java.util.*;
import java.util.function.*;
import org.jetbrains.jwm.*;
import org.jetbrains.skija.*;

public class PanelTextInput extends Panel implements TextInputClient {
    public final Window window;
    public String text = "";
    public EventTextInputMarked lastMarked = null;
    public boolean cursorDraw = true;
    public TimerTask timerTask;
    public static Timer timer = new Timer(true);
    public boolean _wasInside = false;

    public PanelTextInput(Window window) {
        this.window = window;
        window.setTextInputEnabled(true);
        window.setTextInputClient(this);
        timerTask = new TimerTask() {
            public void run() {
                cursorDraw = !cursorDraw;
            }
        };
        timer.schedule(timerTask, 0, 700);
    }

    @Override
    public void accept(Event e) {
        if (e instanceof EventTextInput ee) {
            text += ee.getText();
            lastMarked = null;
        } else if (e instanceof EventTextInputMarked ee) {
            lastMarked = ee;
        } else if (e instanceof EventMouseButton ee) {
            window.unmarkText();
        } else if (e instanceof EventMouseMove ee) {
            boolean isInside = contains(ee.getX(), ee.getY());
            if (!_wasInside && isInside)
                window.setMouseCursor(MouseCursor.IBEAM);
            if (_wasInside && !isInside)
                window.setMouseCursor(MouseCursor.ARROW);
            _wasInside = isInside;
        } else if (e instanceof EventKey ee && ee.isPressed()) {
            Key key = ee.getKey();
            boolean modifier = ee.isModifierDown(Example.MODIFIER);
            if (Key.ENTER == key) {
                text += "\n";
            } else if (Key.BACKSPACE == key) {
                if (lastMarked == null && text.length() > 0) {
                    try (var iter = BreakIterator.makeCharacterInstance();) {
                        iter.setText(text);
                        text = text.substring(0, iter.preceding(text.length()));
                    }
                }
            } else if (Key.C == key && modifier) {
                Clipboard.set(ClipboardEntry.makePlainText(text));
            } else if (Key.V == key && modifier) {
                window.unmarkText();
                ClipboardEntry entry = Clipboard.get(ClipboardFormat.TEXT);
                if (entry != null)
                    text = entry.getString();
            } else if (Key.F == key && modifier) {
                ClipboardFormat[] formats = Clipboard.getFormats();
                if (formats != null)
                    for (ClipboardFormat format: formats)
                        System.out.println(format.getFormatId());
            }
        }
    }

    @Override
    public void paintImpl(Canvas canvas, int width, int height, float scale) {
        try (var paint = new Paint()) {
            canvas.save();
            paint.setColor(0xFFFFFFFF);
            canvas.drawRRect(RRect.makeXYWH(0, 0, width, height, 4 * scale), paint);

            Font font = Example.FONT24;
            FontMetrics metrics = font.getMetrics();

            var y = height - Example.PADDING - metrics.getDescent();
            var lines = text.split("\n", -1);
            var i = lines.length - 1;

            // Last line
            try (var line = TextLine.make(lines[i], font)) {
                canvas.save();
                canvas.translate(Example.PADDING, y);
                paint.setColor(0xFF000000);
                canvas.drawTextLine(line, 0, 0, paint);
                canvas.translate(line.getWidth(), 0);

                // marked text
                paint.setColor(0xFF0087D8);
                if (lastMarked != null) {

                    try (var marked = TextLine.make(lastMarked.getText(), font)) {
                        canvas.drawTextLine(marked, 0, 0, paint);
                        var start = marked.getCoordAtOffset(lastMarked.getSelectionStart());
                        var end = marked.getCoordAtOffset(lastMarked.getSelectionEnd());
                        if (0 < start - 1 * scale)
                            canvas.drawRect(Rect.makeLTRB(0, 2 * scale, start - 1 * scale, 4 * scale), paint);
                        if (start + 1 * scale < end - 1 * scale)
                            canvas.drawRect(Rect.makeLTRB(start + 1 * scale, 2 * scale, end - 1 * scale, 6 * scale), paint);
                        if (end + 1 * scale < marked.getWidth())
                            canvas.drawRect(Rect.makeLTRB(end + 1 * scale, 2 * scale, marked.getWidth(), 4 * scale), paint);

                        canvas.translate(marked.getWidth(), 0);
                    }
                }

                // cursor (cursorDraw used for blink animation)
                if (cursorDraw)
                    canvas.drawRect(Rect.makeXYWH(0, metrics.getAscent(), 2 * scale, metrics.getHeight()), paint);

                canvas.restore();
            }

            paint.setColor(0xFF000000);
            for (i = lines.length - 2; i >= 0; i--) {
                y -= metrics.getHeight();
                try (var line = TextLine.make(lines[i], font)) {
                    canvas.drawTextLine(line, Example.PADDING, y, paint);
                }
            }
            canvas.restore();
        }
    }

    @Override
    public UIRect getRectForMarkedRange(int selectionStart, int selectionEnd) {
        // System.out.println("TextInputClient::getRectForMarkedRange " + selectionStart + ".." + selectionEnd);
        Font font = Example.FONT24;
        FontMetrics metrics = font.getMetrics();

        var lines = text.split("\n", -1);
        try (var line = TextLine.make(lines[lines.length - 1], font);) {
            var left = lastX + Example.PADDING + line.getWidth();
            var top = lastY + lastHeight - Example.PADDING - metrics.getHeight();

            if (lastMarked != null) {
                try (var marked = TextLine.make(lastMarked.getText(), font)) {
                    var start = marked.getCoordAtOffset(selectionStart);
                    var end = marked.getCoordAtOffset(selectionEnd);
                    return UIRect.makeXYWH((int) (left + start),
                                           (int) top,
                                           (int) (end - start),
                                           (int) metrics.getHeight());
                }
            } else
                return UIRect.makeXYWH((int) left, (int) top, 0, (int) metrics.getHeight());
        }
    }
}