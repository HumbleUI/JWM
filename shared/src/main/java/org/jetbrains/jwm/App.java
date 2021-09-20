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
     * <p>Initialize global JWM application instance.</p>
     *
     * <p>This method must be the first method called in the JWM library.</p>
     * <p>After this method call, library API can be safely accessed for creating windows and querying system info.</p>
     */
    public static void init() {
        Library.load();
        _nInit();
        _uiThreadId = Thread.currentThread().getId();
        Log.setVerbose("true".equals(System.getenv("JWM_VERBOSE")));
        Log.setLogger(System.out::println);
    }

    /**
     * <p>Make new native platform-specific window.</p>
     * <p>Note: must be called only after {@link #init()} successful method call.</p>
     *
     * @return          new window instance
     */
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
     * <p>Start application primary event loop.</p>
     *
     * <p>Will block current thread until you call {@link #terminate()}</p>
     * <p>Note: must be called only after {@link #init()} successful method call.</p>
     *
     * @return          status code; 0 on success, otherwise error
     */
    public static int start() {
        assert _onUIThread();
        return _nStart();
    }

    /**
     * <p>Request application terminate.</p>
     * <p>This request causes application terminate and causes control return from {@link #start()} method.</p>
     * <p>Note: must be called only after {@link #init()} successful method call.</p>
     */
    public static void terminate() {
        assert _onUIThread();
        _nTerminate();
    }

    /**
     * <p>Enqueue callback function to run on application UI thread.</p>
     * <p>This is the only method, which cann be access from any thread if other is not specified.</p>
     * <p>Use this method to access UI thread and safely perform user-specific work and access JWM API.</p>
     *
     * @param callback  function for execution on UI thread
     */
    public static void runOnUIThread(Runnable callback) {
        _nRunOnUIThread(callback);
    }

    /**
     * <p>Get desktop environment screens configurations.</p>
     * <p>Note: must be called only after {@link #init()} successful method call.</p>
     *
     * @return          list of desktop screens
     */
    public static Screen[] getScreens() {
        assert _onUIThread();
        return _nGetScreens();
    }

    /**
     * <p>Get desktop environment primary screen info.</p>
     * <p>Note: must be called only after {@link #init()} successful method call.</p>
     *
     * @return          primary desktop screen
     */
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
