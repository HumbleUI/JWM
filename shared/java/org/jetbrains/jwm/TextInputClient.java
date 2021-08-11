package org.jetbrains.jwm;

public interface TextInputClient {
    UIRect rectForMarkedRange(int from, int to);
    // default int characterIndexForPoint(int x, int y) { return 0; } 
}