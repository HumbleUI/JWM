package org.jetbrains.jwm.examples;

import java.util.function.*;
import lombok.*;
import org.jetbrains.annotations.*;
import org.jetbrains.jwm.*;

public class SingleWindow {
    public static void main(String[] args) {
        App.init();
        Window w = new Window() {
            @Override
            public void onEvent(Event e) {
                System.out.println(e);
                if (e instanceof CloseEvent) {
                    close();
                    App.terminate();
                }
            }
        };
        w.show();
        App.runEventLoop();
    }
}