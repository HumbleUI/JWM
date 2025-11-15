package io.github.humbleui.jwm.skija;

import io.github.humbleui.jwm.*;
import io.github.humbleui.skija.*;
import lombok.*;
import org.jetbrains.annotations.*;

public class LayerRasterSkija extends LayerRaster {
    @Getter @Setter @ApiStatus.Internal public ColorInfo _colorInfo = new ColorInfo(ColorType.N32, ColorAlphaType.PREMUL, null);
    @Getter @ApiStatus.Internal public Surface _surface = null;

    @Override
    public void frame() {
        if (_surface == null) {
            ImageInfo imageInfo = new ImageInfo(_colorInfo, getWidth(), getHeight());
            _surface = Surface.wrapPixels(imageInfo, getPixelsPtr(), getRowBytes());
        }

        _window.accept(new EventFrameSkija(_surface));

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
        assert !isClosed();
        if (_surface != null) {
            _surface.close();
            _surface = null;
        }

        super.close();
    }
}