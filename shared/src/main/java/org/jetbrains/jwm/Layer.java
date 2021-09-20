package org.jetbrains.jwm;

public interface Layer extends AutoCloseable {
    /**
     * <p>Attach window to the graphics layer for rendering.</p>
     * <p>Must be called once for single window right after layer is created.</p>
     * <p>If layer fails to attach window, this method throws LayerNotSupportedException.</p>
     *
     * @param window        window to attach
     */
    void attach(Window window);

    /**
     * <p>Reconfigure layer for attached window.</p>
     * <p>Must be called to recreate internal layer platform specific context if window/environment/screen settings changed.</p>
     */
    void reconfigure();

    /**
     * <p>Resize layer framebuffer/area for rendering.</p>
     * <p>Must be called for proper rendering if window content area is resized.</p>
     *
     * @param width         new framebuffer width in pixels
     * @param height        new framebuffer height in pixels
     */
    void resize(int width, int height);

    /**
     * <p>Get current layer framebuffer width in pixels.</p>
     * @return              width in pixels
     */
    int getWidth();

    /**
     * <p>Get current layer framebuffer height in pixels.</p>
     * @return              height in pixels
     */
    int getHeight();

    /**
     * <p>Request back-buffer swap for presentation on this layer.</p>
     * <p>Must be called after each accepted frame event for correct presentation.</p>
     */
    void swapBuffers();

    @Override
    void close();
}