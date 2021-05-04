package org.jetbrains.jwm.examples;

import java.util.function.*;
import lombok.*;
import org.jetbrains.annotations.*;
import org.jetbrains.jwm.*;

public class SingleWindow {
    public static void main(String[] args) {
        App.init();
        Window window = new Window();
        MetalContext context = new MetalContext(true);
        window.attach(context);
        window.setEventListener(e -> {
            System.out.println(e);
            if (e instanceof CloseEvent) {
                window.close();
                App.terminate();
            }
        });
        window.show();
        App.runEventLoop();
    }
}