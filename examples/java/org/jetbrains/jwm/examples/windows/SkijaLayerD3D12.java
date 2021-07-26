package org.jetbrains.jwm.examples.windows;

import org.jetbrains.jwm.*;
import org.jetbrains.jwm.examples.*;
import org.jetbrains.skija.*;

public class SkijaLayerD3D12 extends LayerD3D12 implements SkijaLayer {
    public DirectContext _directContext;
    public BackendRenderTarget _renderTarget;
    public Surface _surface;

    @Override
    public void attach(Window window) {
        super.attach(window);
        //_directContext = DirectContext.makeMetal(getDevicePtr(), getQueuePtr());
    }

    @Override
    public Canvas beforePaint() {
//         _renderTarget = BackendRenderTarget.makeMetal(getWidth(), getHeight(), nextDrawableTexturePtr());
//         _surface = Surface.makeFromBackendRenderTarget(
//                      _directContext,
//                      _renderTarget,
//                      SurfaceOrigin.TOP_LEFT,
//                      SurfaceColorFormat.BGRA_8888,
//                      ColorSpace.getSRGB(),  // TODO load monitor profile
//                      new SurfaceProps(PixelGeometry.RGB_H));
//         return _surface.getCanvas();
        return null;
    }

    @Override
    public void afterPaint() {
        //_surface.flushAndSubmit();
        swapBuffers();

        //_surface.close();
        //_renderTarget.close();
    }

    @Override
    public void close() {
        if (_directContext != null) {
            _directContext.abandon();
            _directContext.close();
        }
        super.close();
    }
}