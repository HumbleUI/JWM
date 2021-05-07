package org.jetbrains.jwm.examples;

import org.jetbrains.jwm.*;
import org.jetbrains.skija.*;

public class SkijaBridgeGL implements SkijaBridge {
    public ContextGL _context;
    public DirectContext _directContext;
    public BackendRenderTarget _renderTarget;
    public Surface _surface;

    public SkijaBridgeGL(Window window, ContextGL context) {
        _context = context;
        window.attach(_context);
    }

    @Override
    public Context getContext() {
        return _context;
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
        int width = _context.getWidth();
        int height = _context.getHeight();

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
        _context.swapBuffers();
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