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
        // ExecutorService _executor = Executors.newSingleThreadExecutor();
        // _executor.submit(() -> {
        //     Window w = new WindowWin32(_executor);
        //     onCreate.accept(w);
        // });
        _nRunOnUIThread(() -> {
            Window w = new WindowWin32(WindowWin32::_nRunOnUIThread);
            onCreate.accept(w);
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
        _executor.execute(runnable);
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
    @ApiStatus.Internal public static native void _nRunOnUIThread(Runnable runnable); // TODO remove
    @ApiStatus.Internal public native void _nClose();
}