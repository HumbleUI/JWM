package org.jetbrains.jwm;

public interface Layer extends AutoCloseable {
    void attach(Window window);

    void reconfigure();

    void resize(int width, int height);

    int getWidth();

    int getHeight();

    void swapBuffers();

    void makeCurrent();

    @Override
    void close();
}