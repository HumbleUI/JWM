package org.jetbrains.jwm;

import lombok.*;
import java.lang.Thread;
import java.lang.reflect.*;
import java.util.*;
import java.util.concurrent.*;
import java.util.function.*;
import org.jetbrains.annotations.*;
import org.jetbrains.jwm.impl.*;

public class App {
    @ApiStatus.Internal
    public static List<Window> _windows = Collections.synchronizedList(new ArrayList<Window>());
    @ApiStatus.Internal
    public static long _uiThreadId;

    /**
     * Call this before you do anything else
     * TODO: make idempotent
     */
    public static void init() {
        Library.load();
        _nInit();
        _uiThreadId = Thread.currentThread().getId();
        Log.setVerbose("true".equals(System.getenv("JWM_VERBOSE")));
        Log.setLogger(System.out::println);
    }

    @NotNull @SneakyThrows
    public static Window makeWindow() {
        assert _onUIThread();
        Class cls;
        if (Platform.CURRENT == Platform.WINDOWS) {
            cls = App.class.forName("org.jetbrains.jwm.WindowWin32");
        } else if (Platform.CURRENT == Platform.MACOS) {
            cls = App.class.forName("org.jetbrains.jwm.WindowMac");
        } else if (Platform.CURRENT == Platform.X11) {
            cls = App.class.forName("org.jetbrains.jwm.WindowX11");
        } else
            throw new RuntimeException("Unsupported platform: " + Platform.CURRENT);
        Constructor<Window> ctor = cls.getDeclaredConstructor();
        Window window = ctor.newInstance();;
        _windows.add(window);
        return window;
    }

    /**
     * Will block until you call {@link #terminate()}
     */
    public static int start() {
        assert _onUIThread();
        return _nStart();
    }

    public static void terminate() {
        assert _onUIThread();
        _nTerminate();
    }

    public static Screen[] getScreens() {
        assert _onUIThread();
        return _nGetScreens();
    }

    public static void runOnUIThread(Runnable callback) {
        // TODO: we need to create concurrent queue in Java side
        // and then in jni fetch objects from queue in order to
        // avoid any concurrency issues in CC. +It will allow us to
        // write all MT code only once in java shared and cc shared!!!!
        _nRunOnUIThread(callback);
    }

    public static Screen getPrimaryScreen() {
        assert _onUIThread();
        for (Screen s: getScreens())
            if (s.isPrimary())
                return s;
        throw new IllegalStateException("Can't find primary screen");
    }

    @ApiStatus.Internal public static boolean _onUIThread() {
        return _uiThreadId == Thread.currentThread().getId();
    }

    @ApiStatus.Internal public static native void _nInit();
    @ApiStatus.Internal public static native int _nStart();
    @ApiStatus.Internal public static native void _nTerminate();
    @ApiStatus.Internal public static native Screen[] _nGetScreens();
    @ApiStatus.Internal public static native void _nRunOnUIThread(Runnable callback);
}