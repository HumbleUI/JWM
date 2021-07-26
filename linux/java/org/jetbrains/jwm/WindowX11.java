package org.jetbrains.jwm;

import java.util.concurrent.*;
import java.util.function.*;
import lombok.*;
import org.jetbrains.annotations.*;
import org.jetbrains.jwm.*;
import org.jetbrains.jwm.impl.*;

public class WindowX11 extends Window {
    @ApiStatus.Internal
    public static void makeOnWindowThread(Consumer<Window> onCreate) {
        ExecutorService executor = Executors.newSingleThreadExecutor();
        executor.submit(() -> {
            WindowX11 w = new WindowX11(executor);
            onCreate.accept(w);
            w._nStart();
            executor.shutdown();
        });
    }

    @ApiStatus.Internal
    public WindowX11(Executor executor) {
        super(_nMake());
        App._windows.add(this);
    }

    @Override
    public void runOnWindowThread(Runnable runnable) {
        _nRunOnWindowThread(runnable);
    }

    @Override
    public native void show();

    @Override 
    public native int getLeft();

    @Override 
    public native int getTop();

    @Override 
    public native int getWidth();

    @Override 
    public native int getHeight();

    @Override 
    public native float getScale();

    @Override
    public native void move(int left, int top);

    @Override
    public native void resize(int width, int height);

    @Override
    public native void requestFrame();

    @Override
    public void close() {
        _nClose();
        super.close();
    }

    @ApiStatus.Internal public static native long _nMake();
    @ApiStatus.Internal public native void _nClose();
    @ApiStatus.Internal public native void _nStart();
    @ApiStatus.Internal public static native void _nRunOnWindowThread(Runnable runnable);
}