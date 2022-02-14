# Getting started

## Dependencies

First, add the latest version of JWM specific to your platform to the dependencies:

Key        | Value
-----------|-----------------------
groupId    | io.github.humbleui
artifactId | jwm
version    | ![version](https://img.shields.io/maven-central/v/io.github.humbleui/jwm)</version>

E.g. for Maven it’ll look like this:

```xml
<dependency>
    <groupId>io.github.humbleui</groupId>
    <artifactId>jwm</artifactId>
    <version>0.3.1</version>
</dependency>
```

## Creating a window and starting an event loop

Init JWM library:

```java
App.start(() -> {
    // Initialization here
});
```

Create a window:

```java
// Inside App::start callback
Window window = App.makeWindow();
window.setTitle("Hello, world!");
```

Set up a listener:

```java
class EventHandler implements Consumer<Event> {
    public final Window window;

    public EventHandler(Window window) {
        this.window = window;
    }
}
```

Write the `accept` function:

```java
@Override
public void accept(Event e) {
    System.out.println(e);

    if (e instanceof EventWindowCloseRequest) {
        window.close();
        App.terminate();
    }
}
```

Assign handler to the window:

```java
window.setEventListener(new EventHandler(window));
```

Display the window:

```java
window.setVisible(true);
```

Start the event loop (will block the thread):

```java
App.start();
```

See it all together in [GettingStarted.java](GettingStarted.java).

## Notes on running the app

On Windows and Linux, just run:

```sh
java -cp jwm-0.3.1.jar:types-0.1.1.jar GettingStarted.java
```

`types-0.1.1.jar` here are from https://github.com/HumbleUI/Types. They will be included as a transitive dependency if you are using Maven or Gradle.

## Positioning the window

You can access information about screen configuration through `App::getScreens` and `App::getPrimaryScreen`.

This will create a window that occupies the left side of the screen and is 800 px wide:

```
Screen screen = App.getPrimaryScreen();
IRect workArea = screen.getWorkArea();
window.setWindowPosition(workArea.getLeft(), workArea.getTop());
float scale = screen.getScale();
window.setWindowSize(800 * scale, workArea.getHeight());
```

All screen coordinates are in one absolute coordinate space. Top left corner of the primary screen usually has (0, 0) coordinate:

![](screen_coordinates.png)

All pixel sizes are also unscaled. They correspond to the physical screen pixels, not “logical” pixels. Note: this is similar to how Windows/Linux work but opposite of what macOS does. JWM abstracts that difference away for you.

To convert “logical” pixels to physical one, multiply by `Screen::getScale()`. E.g. if you want 800×600 window on macOS, do:

```
float scale = window.getScreen().getScale();
window.setWindowSize(800 * scale, 600 * scale);
```

Window position is specified in the same absolute coordinate space. E.g. to move window to another screen just do `Window::setPosition` to a coordinate within that screen bounds:

![](window_rects.png)

## Handling mouse events

```java
@Override
public void accept(Event e) {
    if (e instanceof EventMouseMove ee) {
        // ee.getX() / ee.getY() for window-relative coordinates
    } else if (e instanceof EventMouseScroll ee) {
        // ee.getDeltaX() / ee.getDeltaY() for amount of text lines to scroll
        // See #115
    } else if (e instanceof EventMouseButton ee) {
        // ee.getButton() + ee.getPressed()
    }
}
```

## Handling keyboard events

Simple key handling (e.g. for program shortcuts) via `EventKey`.
Simple text input via `EventTextInput`.
Advanced text input (IME) via `EventTextInputMarked` + `TextInputClient`. See [PanelTextInput.java](https://github.com/HumbleUI/JWM/blob/main/examples/java/PanelTextInput.java) for hints how to handle IME input.

```java
@Override
public void accept(Event e) {
    if (e instanceof EventKey ee) {
        // ee.getKey() + ee.isPressed() for raw keyboard keys
        // Do not use this for text input
    } else if (e instanceof EventTextInput ee) {
        // ee.getText() for text string entered
    } else if (e instanceof EventTextInputMarked ee) {
        // ...
    }
}
```

## Render loop

JWM drawing loop is on-demand and always v-synced with monitor refresh rate (except rare cases like raster rendering on X11). To draw a frame, you first post a frame request:

```java
window.requestFrame();
```

Note that if you do it not from the UI thread (== not from `accept` method of EventHandler), use `App::runOnUIThread` to get to UI thread first:

```java
App.runOnUIThread(() -> window.requestFrame());
```

Then, in `accept` method handle `EventFrame`:

```java
@Override
public void accept(Event e) {
    if (e instanceof EventFrame) {
        paint(window);
    }
}
```

This will render just one frame. If you want to render in a loop (e.g. for an animation or for a game), you can request next frame right after paint:

```java
@Override
public void accept(Event e) {
    if (e instanceof EventFrame) {
        paint(window);
        window.requestFrame();
    }
}
```

## Layers

Now, JWM does not provide anything to actually _draw_ stuff. You can use other libraries, e.g. [Skija](https://github.com/jetbrains/skija), to do that. But JWM provides Layers to initilize graphical context and get all the necessary pointers to use with DirectX 12, OpenGL or Metal.

Let’s write a simple `paint` function that initializes OpenGL and uses it to draw. First, let’s create a layer and `attach` it:

```java
class EventHandler implements Consumer<Event> {
    public final Window window;
    public final LayerGL layer;

    public EventHandler(Window window) {
        this.window = window;
        layer = new LayerGL();
        window.setLayer(layer);
    }
}
```

Then, if the paint request comes, we need to redirect it to the paint function:

```java
@Override
public void accept(Event e) {
    if (e instanceof EventFrame) {
        paint();
        window.requestFrame();
    }
}
```

And finally, we write the paint function itself:

```java
public void paint() {
    layer.makeCurrent();
    // do the drawing
    layer.swapBuffers();
}
```

## Using JWM with Skija

If you wish to use [Skija](https://github.com/HumbleUI/Skija) for rendering, JWM comes with convenient `Layer*Skija` classes:

```
layer = new LayerGLSkija();
window.setLayer(layer);
```

Skija layers work by generating a special event (`EventFrameSkija`) every time frame is requested. So instead of handling `EventFrame`, handle `EventFrameSkija`:

```
if (e instanceof EventFrameSkija ee) {
    Surface s = ee.getSurface();
    paint(s.getCanvas(), s.getWidth(), s.getHeight());
}
```

Note: JWM does not declare Skija as a dependency. For Skija layers to work, add Skija (0.98.0 or later) to your apps’s dependencies.