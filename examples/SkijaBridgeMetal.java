package org.jetbrains.jwm.examples;

import org.jetbrains.jwm.*;
import org.jetbrains.skija.*;

public class SkijaBridgeMetal implements SkijaBridge {
    public ContextMetal _context;
    public DirectContext _directContext;
    public BackendRenderTarget _renderTarget;
    public Surface _surface;

    public SkijaBridgeMetal(Window window, ContextMetal context) {
        _context = context;
        window.attach(_context);
        _directContext = DirectContext.makeMetal(_context.getDevicePtr(), _context.getQueuePtr());
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
        }
    }

    @Override
    public Canvas beforePaint() {
        long mtkViewPtr = _context.nextDrawableTexturePtr();
        float scale = _context.getScale();
        int width = _context.getWidth();
        int height = _context.getHeight();

        // _renderTarget = BackendRenderTarget.makeMetal(width, height, texturePtr);
        _surface = Surface.makeFromMTKView(
                     _directContext,
                     mtkViewPtr,
                     SurfaceOrigin.TOP_LEFT,
                     1,
                     SurfaceColorFormat.BGRA_8888,
                     ColorSpace.getSRGB(),  // TODO load monitor profile
                     null
                     // new SurfaceProps(PixelGeometry.RGB_H)
                     );
        return _surface.getCanvas();
    }

    @Override
    public void afterPaint() {
        _surface.flushAndSubmit();
        _context.swapBuffers();

        _surface.close();
        // _renderTarget.close();
    }

    @Override
    public void resize() {
    }
}