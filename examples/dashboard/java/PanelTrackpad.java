package io.github.humbleui.jwm.examples;

import java.util.*;
import java.util.function.*;
import java.util.stream.*;

import io.github.humbleui.jwm.*;
import io.github.humbleui.skija.*;
import io.github.humbleui.types.*;

public class PanelTrackpad extends Panel {
    public Map<Integer, Point> touches = Collections.synchronizedMap(new HashMap<Integer, Point>()); // coords
    public Map<Integer, Point> devices = Collections.synchronizedMap(new TreeMap<Integer, Point>()); // sizes
    public Map<Integer, Integer> touchDevices = Collections.synchronizedMap(new HashMap<Integer, Integer>()); // touch -> device

    public PanelTrackpad(Window window) {
        super(window);
    }

    @Override
    public void accept(Event e) {
        if (e instanceof EventTrackpadTouchStart ee) {
            touchDevices.put(ee.getDeviceId(), ee.getId());
            devices.put(ee.getDeviceId(), new Point(ee.getDeviceWidth(), ee.getDeviceHeight()));
            touches.put(ee.getId(), new Point(ee.getFracX(), ee.getFracY()));
        } else if (e instanceof EventTrackpadTouchMove ee) {
            touches.put(ee.getId(), new Point(ee.getFracX(), ee.getFracY()));
        } else if (e instanceof EventTrackpadTouchCancel ee) {
            touches.remove(ee.getId());
        } else if (e instanceof EventTrackpadTouchEnd ee) {
            touches.remove(ee.getId());
        }
    }

    @Override
    public void paintImpl(Canvas canvas, int width, int height, float scale) {
    }
}
