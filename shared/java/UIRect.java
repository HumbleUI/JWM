package io.github.humbleui.jwm;

import lombok.*;
import org.jetbrains.annotations.*;

@Getter
@EqualsAndHashCode
public class UIRect {
    public final int _left;
    public final int _top;
    public final int _right;
    public final int _bottom;

    @ApiStatus.Internal
    public UIRect(int l, int t, int r, int b) {
        _left = l;
        _top = t;
        _right = r;
        _bottom = b;
    }

    public int getWidth() {
        return _right - _left;
    }

    public int getHeight() {
        return _bottom - _top;
    }

    @NotNull @Contract("_, _, _, _ -> new")
    public static UIRect makeLTRB(int l, int t, int r, int b) {
        if (l > r)
            throw new IllegalArgumentException("UIRect::makeLTRB expected l <= r, got " + l + " > " + r);
        if (t > b)
            throw new IllegalArgumentException("UIRect::makeLTRB expected t <= b, got " + t + " > " + b);
        return new UIRect(l, t, r, b);
    }

    @NotNull @Contract("_, _, _, _ -> new")
    public static UIRect makeXYWH(int l, int t, int w, int h) {
        if (w < 0)
            throw new IllegalArgumentException("UIRect::makeXYWH expected w >= 0, got: " + w);
        if (h < 0)
            throw new IllegalArgumentException("UIRect::makeXYWH expected h >= 0, got: " + h);
        return w >= 0 && h >= 0 ? new UIRect(l, t, l + w, t + h) : null;
    }

    @NotNull @Contract("_, _ -> new")
    public static UIRect makeWH(int w, int h) {
        if (w < 0)
            throw new IllegalArgumentException("UIRect::makeWH expected w >= 0, got: " + w);
        if (h < 0)
            throw new IllegalArgumentException("UIRect::makeWH expected h >= 0, got: " + h);
        return w >= 0 && h >= 0 ? new UIRect(0, 0, w, h) : null;
    }

    @Nullable
    public UIRect intersect(@NotNull UIRect other) {
        assert other != null : "UIRect::intersect expected other != null";
        if (_right <= other._left || other._right <= _left || _bottom <= other._top || other._bottom <= _top)
            return null;
        return new UIRect(Math.max(_left, other._left), Math.max(_top, other._top), Math.min(_right, other._right), Math.min(_bottom, other._bottom));
    }

    @NotNull
    public UIRect offset(int dx, int dy) {
        return new UIRect(_left + dx, _top + dy, _right + dx, _bottom + dy);
    }

    public boolean contains(int x, int y) {
        return _left <= x && x <= _right && _top <= y && y <= _bottom;
    }

    public String toString() {
        return "(" + _left + "," + _top + ") " + (_right - _left) + "×" + (_bottom - _top);
    }
}