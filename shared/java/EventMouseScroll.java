package io.github.humbleui.jwm;

import lombok.*;
import org.jetbrains.annotations.*;

@Data
public class EventMouseScroll implements Event {
    /**
     * Always has pixel value, but might be an estimation on Windows.
     * Direction ← + / − →
     */
    @ApiStatus.Internal public final float _deltaX;

    /**
     * Always has pixel value to scroll, but might be an estimation on Windows.
     * ↑ + / ↓ −
     */
    @ApiStatus.Internal public final float _deltaY;

    /**
     * On Windows, how many characters to scroll horizontally.
     * Might be used instead of deltaX for more precise scrolling.
     * Always zero on macOS.
     */
    @ApiStatus.Internal public final float _deltaChars;

    /**
     * On Windows, how many lines of content to scroll vertically.
     * Might be used instead of deltaY for more precise scrolling.
     * Always zero on macOS and on Windows if scrolling is set to pages.
     */
    @ApiStatus.Internal public final float _deltaLines;

    /**
     * On Windows, how many whole pages to scroll vertically.
     * Only non-zero on Windows if scrolling is set to pages.
     */
    @ApiStatus.Internal public final float _deltaPages;

    @ApiStatus.Internal @Getter(AccessLevel.NONE) public final int _modifiers;

    public boolean isModifierDown(KeyModifier modifier) {
        return (_modifiers & modifier._mask) != 0;
    }
}