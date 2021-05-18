package org.jetbrains.jwm.examples;

import org.jetbrains.jwm.*;
import org.jetbrains.skija.*;

public class SkijaBridgeGL implements SkijaBridge {
    public LayerGL _layer;
    public DirectContext _directContext;
    public BackendRenderTarget _renderTarget;
    public Surface _surface;

    public SkijaBridgeGL(Window window, LayerGL layer) {
        _layer = layer;
        window.attach(_layer);
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
            _directContext = null;
        }

        if (_surface != null) {
            _surface.close();
            _surface = null;
        }

        if (_renderTarget != null) {
            _renderTarget.close();
            _renderTarget = null;
        }
    }

    @Override
    public Canvas beforePaint() {
        int width = _layer.getWidth();
        int height = _layer.getHeight();

        if (_directContext == null)
            _directContext = DirectContext.makeGL();

        if (_renderTarget == null)
            _renderTarget = BackendRenderTarget.makeGL(
                             width,
                             height,
                             /*samples*/0,
                             /*stencil*/8,
                             /*fbId*/0,
                             FramebufferFormat.GR_GL_RGBA8);

        if (_surface == null)
            _surface = Surface.makeFromBackendRenderTarget(
                        _directContext,
                        _renderTarget,
                        SurfaceOrigin.BOTTOM_LEFT,
                        SurfaceColorFormat.RGBA_8888,
                        ColorSpace.getSRGB(),  // TODO load monitor profile
                        new SurfaceProps(PixelGeometry.RGB_H));

        return _surface.getCanvas();
    }

    @Override
    public void afterPaint() {
        _surface.flushAndSubmit();
        _layer.swapBuffers();
    }

    @Override
    public void resize() {
        if (_surface != null) {
            _surface.close();
            _surface = null;
        }
        if (_renderTarget != null) {
            _renderTarget.close();
            _renderTarget = null;
        }
        if (_directContext != null) {
            _directContext.abandon();
            _directContext.close();
            _directContext = null;
        }
   }
}