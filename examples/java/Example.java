package io.github.humbleui.jwm.examples;

import java.util.function.Consumer;

import io.github.humbleui.jwm.App;
import io.github.humbleui.jwm.Event;
import io.github.humbleui.jwm.EventFrame;
import io.github.humbleui.jwm.EventWindowCloseRequest;
import io.github.humbleui.jwm.EventWindowResize;
import io.github.humbleui.jwm.EventWindowScreenChange;
import io.github.humbleui.jwm.LayerGL;
import io.github.humbleui.jwm.UIRect;
import io.github.humbleui.jwm.Window;
import io.github.humbleui.jwm.WindowWin32;
public class Example {
    public static void main(String[] args) {
        App.init();

        // Using the generic window seems to cause:
        // Exception in thread "main" java.lang.NoSuchMethodException: io.github.humbleui.jwm.WindowWin32.<init>()
        //     at java.lang.Class.getConstructor0(DynamicHub.java:3585)
        //     at java.lang.Class.getDeclaredConstructor(DynamicHub.java:2754)
        //     at io.github.humbleui.jwm.App.makeWindow(App.java:50)
        //     at io.github.humbleui.jwm.examples.Example.main(Example.java:18)

        // Maybe because the underlying constructor isn't directly invoked, so Graal doesn't statically analyze it?
        // Potential solution: Hard-code "if OS == WINDOWS { new WindowWin32() }" etc?

        // Window window = App.makeWindow();
        WindowWin32 window = new WindowWin32();
        window.setEventListener(new EventHandler(window));
        window.setVisible(true);
        window.requestFrame();
        App.start();
    }
}

class EventHandler implements Consumer<Event> {
    public final Window window;
    public final LayerGL layer;

    public EventHandler(Window window) {
        this.window = window;
        layer = new LayerGL();
        layer.attach(window);
    }

    @Override
    public void accept(Event e) {
        System.out.println(e);

        if (e instanceof EventWindowCloseRequest) {
            window.close();
            App.terminate();
        } else if (e instanceof EventWindowScreenChange) {
            layer.reconfigure();
            UIRect contentRect = window.getContentRect();
            layer.resize(contentRect.getWidth(), contentRect.getHeight());
            paint();
        } else if (e instanceof EventWindowResize ee) {
            layer.resize(ee.getContentWidth(), ee.getContentHeight());
            paint();
        } else if (e instanceof EventFrame) {
            paint();
            window.requestFrame();
        }
    }

    public void paint() {
        layer.makeCurrent();
        // do the drawing
        layer.swapBuffers();
    }
}
