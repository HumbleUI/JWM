package io.github.humbleui.jwm;

import io.github.humbleui.types.*;

public interface TextInputClient {
    /**
     * <p>Get UI rect in screen space for currently edited text region.</p>
     *
     * <p>This method is called if user currently in complex text input and IME mode types complex characters.</p>
     * <p>Returned rect is used to correctly position IME and system pop-up windows and elements nearly the edited text region.</p>
     *
     * @param selectionStart    edited text region string start
     * @param selectionEnd      edited text region string end
     * @return                  ui screen rect
     *
     * @see <a href="https://github.com/HumbleUI/JWM/blob/main/examples/java/io/github/humbleui/jwm/examples/PanelTextInput.java">Text input example</a>
     */
    IRect getRectForMarkedRange(int selectionStart, int selectionEnd);

    /**
     * Returns the range of selected text
     */
    IRange getSelectedRange();

    /**
     * Returns the range of marked text
     */
    IRange getMarkedRange();

    /**
     * Returns part of the edited string
     */
    String getSubstring(int start, int end);
}