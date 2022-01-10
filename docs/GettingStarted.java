import java.util.function.Consumer;
import io.github.humbleui.jwm.*;

public class GettingStarted {
    public static void main(String[] args) {
        App.init();
        Window window = App.makeWindow();
        window.setEventListener(new EventHandler(window));
        window.setVisible(true);
        App.start();
    }
}

class EventHandler implements Consumer<Event> {
    public final Window window;
    public final LayerGL layer;

    public EventHandler(Window window) {
        this.window = window;
        layer = new LayerGL();
        window.setLayer(layer);
    }

    @Override
    public void accept(Event e) {
        System.out.println(e);
        if (e instanceof EventWindowCloseRequest) {
            window.close();
            App.terminate();
        } else if (e instanceof EventFrame) {
            paint();
        }
    }

    public void paint() {
        layer.makeCurrent();
        // do the drawing
        layer.swapBuffers();
    }
}
