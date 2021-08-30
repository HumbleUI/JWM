import java.util.function.Consumer;
import org.jetbrains.jwm.*;

public class GettingStarted {
    public static void main(String[] args) {
        App.init();
        Window window = App.makeWindow();
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
