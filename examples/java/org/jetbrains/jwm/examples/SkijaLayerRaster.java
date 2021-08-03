package org.jetbrains.jwm.examples;

import org.jetbrains.jwm.LayerRaster;
import org.jetbrains.skija.*;

public class SkijaLayerRaster extends LayerRaster implements SkijaLayer {
    public Surface _surface;

    @Override
    public Canvas beforePaint() {
        if (_surface == null) {
            ImageInfo imageInfo = ImageInfo.makeN32Premul(getWidth(), getHeight());
            _surface = Surface.makeRasterDirect(imageInfo, getPixelsPtr(), getRowBytes());
        }

        Canvas canvas = _surface.getCanvas();
        canvas.clear(0xFFFFFFFF);
        return canvas;
    }

    @Override
    public void afterPaint() {
        swapBuffers();
    }

    @Override
    public void resize(int width, int height) {
        if (_surface != null) {
            _surface.close();
            _surface = null;
        }

        super.resize(width, height);
    }

    @Override
    public void close() {
        if (_surface != null) {
            _surface.close();
            _surface = null;
        }

        super.close();
    }
}