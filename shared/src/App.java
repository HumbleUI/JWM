package org.jetbrains.jwm;

import org.jetbrains.annotations.*;

public class App {
    static { Library.staticLoad(); }

    public static void init() {
        _nInit();
    }

    public static int runEventLoop() {
        return _nRunEventLoop();
    }

    public static void terminate() {
        _nTerminate();
    }
   
    @ApiStatus.Internal public static native void _nInit();
    @ApiStatus.Internal public static native int  _nRunEventLoop();
    @ApiStatus.Internal public static native void _nTerminate();
}