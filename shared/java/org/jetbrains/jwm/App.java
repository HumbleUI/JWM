package org.jetbrains.jwm;

import java.lang.reflect.*;
import java.util.*;
import java.util.concurrent.*;
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
    public static void makeWindow(Consumer<Window> onCreate) {
        Class cls;
        if (Platform.CURRENT == Platform.WINDOWS) {
            cls = App.class.forName("org.jetbrains.jwm.WindowWin32");
        } else if (Platform.CURRENT == Platform.MACOS) {
            cls = App.class.forName("org.jetbrains.jwm.WindowMac");
        } else if (Platform.CURRENT == Platform.X11) {
            cls = App.class.forName("org.jetbrains.jwm.WindowX11");
        } else
            throw new RuntimeException("Unsupported platform: " + Platform.CURRENT);
        Method factory = cls.getDeclaredMethod("makeOnWindowThread", Consumer.class);
        factory.invoke(null, onCreate);
    }

    /**
     * Will block until you call {@link #terminate()}
     */
    public static native int start();

    public static native void terminate();

    /**
     * @return contents of the system clipboard. null if empty
     */
    public static native DataTransfer getClipboard();

    @ApiStatus.Internal public static native void _nInit();

    public static native Screen[] getScreens();

    public static Screen getPrimaryScreen() {
        for (Screen s: getScreens())
            if (s.isPrimary())
                return s;
        throw new IllegalStateException("Can't find primary screen");
    }
}