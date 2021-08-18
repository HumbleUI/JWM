package org.jetbrains.jwm;

import org.jetbrains.annotations.ApiStatus;
import org.jetbrains.jwm.impl.RefCounted;

public class LayerD3D12 extends RefCounted implements Layer {
    @ApiStatus.Internal public Window _window;
    @ApiStatus.Internal public int _width;
    @ApiStatus.Internal public int _height;

    public LayerD3D12() {
        super(_nMake());
    }

    @Override
    public void attach(Window window) {
        assert _isValidCall();
        _window = window;
        _nAttach(window);
    }

    @Override
    public void reconfigure() {
        assert _isValidCall();
        _nReconfigure();
    }

    @Override
    public void resize(int width, int height) {
        assert _isValidCall();
        _width = width;
        _height = height;
        _nResize(width, height);
    }

    @Override
    public int getWidth() {
        assert _isValidCall();
        return _width;
    }

    @Override
    public int getHeight() {
        assert _isValidCall();
        return _height;
    }

    @Override
    public void swapBuffers() {
        assert _isValidCall();
        _nSwapBuffers();
    }

    @Override
    public void close() {
        assert _isValidCall();
        _nClose();
        _window = null;
        super.close();
    }

    public long getAdapterPtr() {
        assert _isValidCall();
        return _nGetAdapterPtr();
    }

    public long getDevicePtr() {
        assert _isValidCall();
        return _nGetDevicePtr();
    }

    public long getQueuePtr() {
        assert _isValidCall();
        return _nGetQueuePtr();
    }

    public int getFormat() {
        assert _isValidCall();
        return _nGetFormat();
    }

    public int getSampleCount() {
        assert _isValidCall();
        return _nGetSampleCount();
    }

    public int getLevelCount() {
        assert _isValidCall();
        return _nGetLevelCount();
    }

    public long nextDrawableTexturePtr() {
        assert _isValidCall();
        return _nNextDrawableTexturePtr();
    }

    @ApiStatus.Internal public static boolean _isValidCall() {
        return App._isValidCall();
    }

    @ApiStatus.Internal public static native long _nMake();
    @ApiStatus.Internal public native void _nAttach(Window window);
    @ApiStatus.Internal public native void _nReconfigure();
    @ApiStatus.Internal public native void _nResize(int width, int height);
    @ApiStatus.Internal public native void _nSwapBuffers();
    @ApiStatus.Internal public native long _nGetAdapterPtr();
    @ApiStatus.Internal public native long _nGetDevicePtr();
    @ApiStatus.Internal public native long _nGetQueuePtr();
    @ApiStatus.Internal public native int _nGetFormat();
    @ApiStatus.Internal public native int _nGetSampleCount();
    @ApiStatus.Internal public native int _nGetLevelCount();
    @ApiStatus.Internal public native long _nNextDrawableTexturePtr();
    @ApiStatus.Internal public native void _nClose();
}