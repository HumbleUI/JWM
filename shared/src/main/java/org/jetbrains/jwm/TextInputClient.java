package org.jetbrains.jwm;

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
     * @see <a href="https://github.com/HumbleUI/JWM/blob/main/examples/java/org/jetbrains/jwm/examples/PanelTextInput.java">Text input example</a>
     */
    UIRect getRectForMarkedRange(int selectionStart, int selectionEnd);

    // todo: remove default int characterIndexForPoint(int x, int y) { return 0; }
}