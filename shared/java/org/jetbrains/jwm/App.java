package org.jetbrains.jwm;

import lombok.SneakyThrows;
import org.jetbrains.annotations.ApiStatus;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.jwm.impl.Library;

import java.lang.reflect.Constructor;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

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
    public static Window makeWindow() {
        Class cls;
        if (Platform.CURRENT == Platform.WINDOWS)
            cls = App.class.forName("org.jetbrains.jwm.WindowWin32");
        else if (Platform.CURRENT == Platform.MACOS)
            cls = App.class.forName("org.jetbrains.jwm.WindowMac");
        else if (Platform.CURRENT == Platform.X11)
            cls = App.class.forName("org.jetbrains.jwm.WindowX11");
        else
            throw new RuntimeException("Unsupported platform: " + Platform.CURRENT);
        Constructor<Window> ctor = cls.getDeclaredConstructor();
        Window window = ctor.newInstance();
        _windows.add(window);
        return window;
    }

    /**
     * Will block until you call {@link #terminate()}
     */
    public static native int start();

    public static native void runOnUIThread(Runnable callback);

    public static native void terminate();

    public static native Screen[] getAllScreens();

    public static native Screen getPrimaryScreen();

    @ApiStatus.Internal public static native void _nInit();
}