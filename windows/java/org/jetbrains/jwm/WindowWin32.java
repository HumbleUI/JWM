package org.jetbrains.jwm;

import java.util.concurrent.*;
import java.util.function.*;
import lombok.*;
import org.jetbrains.annotations.*;
import org.jetbrains.jwm.*;
import org.jetbrains.jwm.impl.*;

public class WindowWin32 extends Window {
    @ApiStatus.Internal
    public static void makeOnWindowThread(Consumer<Window> onCreate) {
        ExecutorService _executor = Executors.newSingleThreadExecutor();
        _executor.submit(() -> {
            // Create window on window thread (where it is going to be)
            WindowWin32 w = new WindowWin32(_executor);
            // User provided lambda to setup window
            onCreate.accept(w);
            // Enter window event loop (blocks until window or add are closed)
            w._nStart();
            // Force executor to shutdown (terminate its thread)
            _executor.shutdown();
        });
    }

    @ApiStatus.Internal public final Executor _executor;

    @ApiStatus.Internal
    public WindowWin32(Executor executor) {
        super(_nMake());
        _executor = executor;
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
    @ApiStatus.Internal public native void _nStart();
    @ApiStatus.Internal public native void _nRunOnWindowThread(Runnable runnable);
    @ApiStatus.Internal public native void _nClose();
}