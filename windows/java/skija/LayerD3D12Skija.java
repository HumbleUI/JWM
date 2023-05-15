package io.github.humbleui.jwm.skija;

import io.github.humbleui.jwm.*;
import io.github.humbleui.skija.*;
import lombok.*;
import org.jetbrains.annotations.*;

public class LayerD3D12Skija extends LayerD3D12 {
    @Getter @Setter @ApiStatus.Internal public SurfaceOrigin _origin = SurfaceOrigin.TOP_LEFT;
    @Getter @Setter @ApiStatus.Internal public SurfaceColorFormat _colorFormat = SurfaceColorFormat.RGBA_8888;
    @Getter @Setter @ApiStatus.Internal public ColorSpace _colorSpace = ColorSpace.getSRGB(); // TODO load monitor profile
    @Getter @Setter @ApiStatus.Internal public SurfaceProps _surfaceProps = new SurfaceProps(PixelGeometry.RGB_H);

    @Getter @ApiStatus.Internal public DirectContext _directContext = null;

    @Override
    public void attach(Window window) {
        super.attach(window);
        _directContext = DirectContext.makeDirect3D(getAdapterPtr(), getDevicePtr(), getQueuePtr());
    }

    @Override
    public void frame() {
        try (BackendRenderTarget _renderTarget = BackendRenderTarget.makeDirect3D(_width, _height, nextDrawableTexturePtr(), getFormat(), getSampleCount(), getLevelCount());
             Surface _surface = Surface.makeFromBackendRenderTarget( _directContext, _renderTarget, _origin, _colorFormat, _colorSpace, _surfaceProps);)
        {
            _window.accept(new EventFrameSkija(_surface));

            _surface.flushAndSubmit();
            swapBuffers();

            // todo: call flush for present (instead of default flush)
            // since we want to have BackendSurfaceAccess::kPresent final render target layout
            _surface.flush();
            _directContext.submit(false);

            swapBuffers();
        }
    }

    @Override
    public void resize(int width, int height) {
        // HACK: if direct context is not null, we must call submit here
        // to clean up all outstanding resources in the command queue
        _directContext.submit(true);

        // Resize layer (native call to resize swap chain images)
        super.resize(width, height);
    }

    @Override
    public void close() {
        assert !isClosed() : "Layer is already closed";
        if (_directContext != null) {
            _directContext.abandon();
            _directContext.close();
        }

        super.close();
    }
}