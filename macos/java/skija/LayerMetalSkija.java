package io.github.humbleui.jwm.skija;

import io.github.humbleui.jwm.*;
import io.github.humbleui.skija.*;
import lombok.*;
import org.jetbrains.annotations.*;

public class LayerMetalSkija extends LayerMetal {
    @Getter @Setter @ApiStatus.Internal public SurfaceOrigin _origin = SurfaceOrigin.TOP_LEFT;
    @Getter @Setter @ApiStatus.Internal public SurfaceColorFormat _colorFormat = SurfaceColorFormat.BGRA_8888;
    @Getter @Setter @ApiStatus.Internal public ColorSpace _colorSpace = null;
    @Getter @Setter @ApiStatus.Internal public SurfaceProps _surfaceProps = new SurfaceProps(PixelGeometry.UNKNOWN);

    @Getter @ApiStatus.Internal public DirectContext _directContext;

    @Override
    public void attach(Window window) {
        super.attach(window);
        _directContext = DirectContext.makeMetal(getDevicePtr(), getQueuePtr());
    }

    @Override
    public void frame() {
        if (_colorSpace == null) {
            _colorSpace = LayerSkija.windowColorSpace(_window);
        }

        try (BackendRenderTarget _renderTarget = BackendRenderTarget.makeMetal(_width, _height, nextDrawableTexturePtr());
             Surface _surface = Surface.wrapBackendRenderTarget(_directContext, _renderTarget, _origin, _colorFormat, _colorSpace, _surfaceProps);)
        {
            _window.accept(new EventFrameSkija(_surface));

            _directContext.flushAndSubmit(_surface);
            swapBuffers();
        }
    }

    @Override
    public void reconfigure() {
        if (_colorSpace != null) {
            _colorSpace.close();
        };
        _colorSpace = null;
        super.reconfigure();
    }

    @Override
    public void close() {
        assert !isClosed();
        if (_directContext != null) {
            _directContext.abandon();
            _directContext.close();
        }
        super.close();
    }
}