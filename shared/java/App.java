package org.jetbrains.jwm;

import java.util.*;
import java.util.function.*;
import lombok.*;
import org.jetbrains.annotations.*;
import org.jetbrains.jwm.impl.*;

public class App {
    @ApiStatus.Internal
    public static List<Window> _windows = Collections.synchronizedList(new ArrayList<Window>());

    public static void init() {
        Library.load();
        _nInit();
    }

    @SneakyThrows
    public static Window makeWindow() {
        Window window;
        if (Platform.CURRENT == Platform.MACOS)
            window = (Window) App.class.forName("org.jetbrains.jwm.macos.WindowMac").getDeclaredConstructor().newInstance();
        else
            throw new RuntimeException("Unsupported platform: " + Platform.CURRENT);
        _windows.add(window);
        return window;
    }

    public static int runEventLoop() {
        return _nRunEventLoop(() -> {
            synchronized (_windows) {
                for (var window: _windows)
                    if (window._eventListener != null)
                        window._eventListener.accept(EventPaint.INSTANCE);
            }
        });
    }

    public static native void terminate();

    @ApiStatus.Internal public static native int _nRunEventLoop(Runnable onIdle);
    @ApiStatus.Internal public static native void _nInit();
}