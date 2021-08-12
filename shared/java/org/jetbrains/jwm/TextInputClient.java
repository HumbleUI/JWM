package org.jetbrains.jwm;

public interface TextInputClient {
    UIRect getRectForMarkedRange(int selectionStart, int selectionEnd);
    // default int characterIndexForPoint(int x, int y) { return 0; } 
}