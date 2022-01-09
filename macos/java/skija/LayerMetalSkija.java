package io.github.humbleui.jwm.skija;

import io.github.humbleui.jwm.*;
import io.github.humbleui.skija.*;
import lombok.*;
import org.jetbrains.annotations.*;

public class LayerMetalSkija extends LayerMetal {
    @Getter @Setter @ApiStatus.Internal public SurfaceOrigin _origin = SurfaceOrigin.TOP_LEFT;
    @Getter @Setter @ApiStatus.Internal public SurfaceColorFormat _colorFormat = SurfaceColorFormat.BGRA_8888;
    @Getter @Setter @ApiStatus.Internal public ColorSpace _colorSpace = ColorSpace.getSRGB(); // TODO load monitor profile
    @Getter @Setter @ApiStatus.Internal public SurfaceProps _surfaceProps = new SurfaceProps(PixelGeometry.RGB_H);

    @Getter @ApiStatus.Internal public DirectContext _directContext;

    @Override
    public void attach(Window window) {
        super.attach(window);
        _directContext = DirectContext.makeMetal(getDevicePtr(), getQueuePtr());
    }

    @Override
    public void frame() {
        try (BackendRenderTarget _renderTarget = BackendRenderTarget.makeMetal(_width, _height, nextDrawableTexturePtr());
             Surface _surface = Surface.makeFromBackendRenderTarget(_directContext, _renderTarget, _origin, _colorFormat, _colorSpace, _surfaceProps);)
        {
            _window.accept(new EventFrameSkija(_surface));

            _surface.flushAndSubmit();
            swapBuffers();
        }
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