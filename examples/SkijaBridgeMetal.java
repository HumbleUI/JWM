package org.jetbrains.jwm.examples;

import org.jetbrains.jwm.*;
import org.jetbrains.skija.*;

public class SkijaBridgeMetal implements SkijaBridge {
    public LayerMetal _layer;
    public DirectContext _directContext;
    public BackendRenderTarget _renderTarget;
    public Surface _surface;

    public SkijaBridgeMetal(Window window, LayerMetal layer) {
        _layer = layer;
        window.attach(_layer);
        _directContext = DirectContext.makeMetal(_layer.getDevicePtr(), _layer.getQueuePtr());
    }

    @Override
    public Layer getLayer() {
        return _layer;
    }

    @Override
    public void close() {
        if (_directContext != null) {
            _directContext.abandon();
            _directContext.close();
        }
    }

    @Override
    public Canvas beforePaint() {
        long texturePtr = _layer.nextDrawableTexturePtr();
        float scale = _layer.getScale();
        int width = _layer.getWidth();
        int height = _layer.getHeight();

        _renderTarget = BackendRenderTarget.makeMetal(width, height, texturePtr);
        _surface = Surface.makeFromBackendRenderTarget(
                     _directContext,
                     _renderTarget,
                     SurfaceOrigin.TOP_LEFT,
                     SurfaceColorFormat.BGRA_8888,
                     ColorSpace.getSRGB(),  // TODO load monitor profile
                     new SurfaceProps(PixelGeometry.RGB_H));
        return _surface.getCanvas();
    }

    @Override
    public void afterPaint() {
        _surface.flushAndSubmit();
        _layer.swapBuffers();

        _surface.close();
        _renderTarget.close();
    }

    @Override
    public void resize() {
    }
}