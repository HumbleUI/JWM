package org.jetbrains.jwm;

import java.lang.reflect.*;
import java.util.*;
import java.util.function.*;
import lombok.*;
import org.jetbrains.annotations.*;
import org.jetbrains.jwm.impl.*;

public class App {
    @ApiStatus.Internal
    public static List<Window> _windows = Collections.synchronizedList(new ArrayList<Window>());

    /**
     * Call this before you do anything else
     * TODO: make idempotent
     */
    public static void init() {
        Library.load();
        _nInit();
    }

    @NotNull @SneakyThrows
    public static Window makeWindow(@Nullable WindowOpts opts) {
        Class cls;
        if (Platform.CURRENT == Platform.MACOS)
            cls = App.class.forName("org.jetbrains.jwm.WindowMac");
        else
            throw new RuntimeException("Unsupported platform: " + Platform.CURRENT);
        Constructor<Window> ctor = cls.getDeclaredConstructor(WindowOpts.class);
        Window window = ctor.newInstance(opts);
        _windows.add(window);
        return window;
    }

    /**
     * Will block until you call {@link #terminate()}
     */
    public static native int run();

    public static native void terminate();

    @ApiStatus.Internal public static native void _nInit();
}