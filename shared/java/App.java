package org.jetbrains.jwm;

import java.util.function.*;
import org.jetbrains.annotations.*;
import org.jetbrains.jwm.impl.*;

public class App {
    static { Library.staticLoad(); }

    public static native void init();

    public static native int runEventLoop(Consumer<Event> consumer);

    public static native void terminate();
}