#include "Pointer.hh"
#include "WindowManagerWayland.hh"
#include "WindowWayland.hh"
#include "MouseButtonWayland.hh"
#include "AppWayland.hh"
#include "KeyWayland.hh"
#include <impl/Library.hh>
#include <linux/input-event-codes.h>
using namespace jwm;

static void pointerEnter(void* data, wl_pointer* pointer, uint32_t serial,
            wl_surface* surface, wl_fixed_t surface_x, wl_fixed_t surface_y
        )
{
    auto self = reinterpret_cast<Pointer*>(data);
    self->_serial = serial;
    if (auto window = self->_wm.getWindowForNative(surface)) {
        self->_focusedSurface = jwm::ref(window);
        window->setCursor(jwm::MouseCursor::ARROW);
    }
}

static void pointerLeave(void* data, wl_pointer* pointer, uint32_t serial,
        wl_surface* surface) 
{
    auto self = reinterpret_cast<Pointer*>(data);
    if (self->_focusedSurface)
        jwm::unref(&self->_focusedSurface);

    self->_mouseMask = 0;
    self->_serial = 0;
}
static void pointerMotion(void* data, wl_pointer* pointer, uint32_t time, 
        wl_fixed_t surface_x, wl_fixed_t surface_y) 
{
    auto self = reinterpret_cast<Pointer*>(data);
    self->mouseUpdateUnscaled(wl_fixed_to_int(surface_x), wl_fixed_to_int(surface_y), self->_mouseMask);
}

static void pointerButton(void* data, wl_pointer* pointer, uint32_t serial,
        uint32_t time, uint32_t button, uint32_t state) 
{
    using namespace classes;
    auto self = reinterpret_cast<Pointer*>(data);
    auto window = self->_focusedSurface;
    if (!window) return;
    if (state == 0) {
        // release
        switch (button) {
            // primary
            case BTN_LEFT:
                self->_mouseMask &= ~0x100;
                break;
            // secondary
            case BTN_RIGHT:
                self->_mouseMask &= ~0x400;
                break;
            // middle
            case BTN_MIDDLE:
                self->_mouseMask &= ~0x200;
                break;
            default:
                break;
        }
        
        if (MouseButtonWayland::isButton(button)) {
            jwm::JNILocal<jobject> eventButton(
                    app.getJniEnv(),
                    jwm::classes::EventMouseButton::make(
                            app.getJniEnv(),
                            MouseButtonWayland::fromNative(button),
                            false,
                            self->_lastMouseX,
                            self->_lastMouseY,
                            jwm::KeyWayland::getModifiers(self->_wm.getXkbState())
                        )
                    );
            window->dispatch(eventButton.get());
        }
    } else {
        // down
        switch (button) {
            // primary
            case BTN_LEFT:
                self->_mouseMask |= 0x100;
                break;
            // secondary
            case BTN_RIGHT:
                self->_mouseMask |= 0x400;
                break;
            // middle
            case BTN_MIDDLE:
                self->_mouseMask |= 0x200;
                break;
            default:
                break;
        }
        
        if (MouseButtonWayland::isButton(button)) {
            jwm::JNILocal<jobject> eventButton(
                    app.getJniEnv(),
                    jwm::classes::EventMouseButton::make(
                            app.getJniEnv(),
                            MouseButtonWayland::fromNative(button),
                            true,
                            self->_lastMouseX,
                            self->_lastMouseY,
                            jwm::KeyWayland::getModifiers(self->_wm.getXkbState())
                        )
                    );
            window->dispatch(eventButton.get());
        }
    }
}

static void pointerAxis(void* data, wl_pointer* pointer, uint32_t time,
        uint32_t axis, wl_fixed_t value)
{
    auto self = reinterpret_cast<Pointer*>(data);
    if (!self->_focusedSurface) return;
    float fvalue = static_cast<float>(wl_fixed_to_double(value));
    switch (axis) {
        case WL_POINTER_AXIS_VERTICAL_SCROLL:
            self->_dY += fvalue;
            break;
        case WL_POINTER_AXIS_HORIZONTAL_SCROLL:
            self->_dX += fvalue;
            break;
        default:
            break;
    }
}

static void pointerFrame(void* data, wl_pointer* pointer) 
{
    auto self = reinterpret_cast<Pointer*>(data);
    auto win = self->_focusedSurface;
    if (!win) return;
    if (self->_dX != 0.0f || self->_dY != 0.0f) {
        auto env = app.getJniEnv();
        
        jwm::JNILocal<jobject> eventAxis(
                    env,
                    jwm::classes::EventMouseScroll::make(
                            env,
                            self->_dX * win->_scale,
                            self->_dY * win->_scale,
                            0.0f,
                            0.0f,
                            0.0f,
                            self->_lastMouseX,
                            self->_lastMouseY,
                            jwm::KeyWayland::getModifiers(self->_wm.getXkbState())
                        )
                );
        win->dispatch(eventAxis.get());

        self->_dX = 0.0f;
        self->_dY = 0.0f;
    }
}

static void pointerAxisSource(void* data, wl_pointer* pointer, uint32_t source) {}
static void pointerAxisStop(void* data, wl_pointer* pointer, uint32_t time, uint32_t axis) {}
static void pointerAxisDiscrete(void* data, wl_pointer* pointer, uint32_t axis, int discrete) {}

wl_pointer_listener Pointer::_pointerListener = {
    .enter = pointerEnter,
    .leave = pointerLeave,
    .motion = pointerMotion,
    .button = pointerButton,
    .axis = pointerAxis,
    .frame = pointerFrame,
    .axis_source = pointerAxisSource,
    .axis_stop = pointerAxisStop,
    .axis_discrete = pointerAxisDiscrete
};

Pointer::Pointer(wl_pointer* pointer, WindowManagerWayland* wm):
    _pointer(pointer),
    _wm(*wm) 
{
    _surface = wl_compositor_create_surface(_wm.compositor);
    wl_pointer_add_listener(pointer, &_pointerListener, this);
}

Pointer::~Pointer()
{
    if (_pointer)
        wl_pointer_release(_pointer);
}

void Pointer::mouseUpdate(uint32_t x, uint32_t y, uint32_t mask) {
    auto window = _focusedSurface;
    if (!window)
        return;
    if (_lastMouseX == x && _lastMouseY == y)
        return;
    _lastMouseX = x;
    _lastMouseY = y;
    int movementX = 0, movementY = 0;

    jwm::JNILocal<jobject> eventMove(
        app.getJniEnv(),
        jwm::classes::EventMouseMove::make(app.getJniEnv(),
            x,
            y,
            movementX,
            movementY,
            jwm::MouseButtonWayland::fromNativeMask(mask),
            // impl me!
            jwm::KeyWayland::getModifiers(_wm.getXkbState())
            )
        );
    window->dispatch(eventMove.get());
}

void Pointer::mouseUpdateUnscaled(uint32_t x, uint32_t y, uint32_t mask) {
    auto window = _focusedSurface;
    if (!window) return;
    mouseUpdate(x * window->_scale, y * window->_scale, mask);
}

void Pointer::updateHotspot(int x, int y) {
    if (!_focusedSurface) return;
    wl_pointer_set_cursor(_pointer, _serial, _surface, x / _focusedSurface->_scale, y / _focusedSurface->_scale);
}

wl_cursor_theme* Pointer::_makeCursors(int scale) {
    auto theme = wl_cursor_theme_load(nullptr, 24 * scale, _wm.shm);
    _cursorThemes[scale] = theme;
    return theme;
}

wl_cursor_theme* Pointer::getThemeFor(int scale) {
    auto it = _cursorThemes.find(scale);
    if (it != _cursorThemes.end())
        return it->second;
    return _makeCursors(scale);
}
wl_cursor* Pointer::getCursorFor(int scale, jwm::MouseCursor cursor) {
    auto theme = getThemeFor(scale);

    switch (cursor) {
        case jwm::MouseCursor::ARROW:
            return wl_cursor_theme_get_cursor(theme, "default");
        case jwm::MouseCursor::CROSSHAIR:
            return wl_cursor_theme_get_cursor(theme, "crosshair");
        case jwm::MouseCursor::HELP:
            return wl_cursor_theme_get_cursor(theme, "help");
        case jwm::MouseCursor::POINTING_HAND:
            return wl_cursor_theme_get_cursor(theme, "pointer");
        case jwm::MouseCursor::IBEAM:
            return wl_cursor_theme_get_cursor(theme, "text");
        case jwm::MouseCursor::NOT_ALLOWED:
            return wl_cursor_theme_get_cursor(theme, "not-allowed");
        case jwm::MouseCursor::WAIT:
            return wl_cursor_theme_get_cursor(theme, "watch");
        case jwm::MouseCursor::WIN_UPARROW:
            return wl_cursor_theme_get_cursor(theme, "up-arrow");
        case jwm::MouseCursor::RESIZE_NS:
            return wl_cursor_theme_get_cursor(theme, "ns-resize");
        case jwm::MouseCursor::RESIZE_WE:
            return wl_cursor_theme_get_cursor(theme, "ew-resize");
        case jwm::MouseCursor::RESIZE_NESW:
            return wl_cursor_theme_get_cursor(theme, "nesw-resize");
        case jwm::MouseCursor::RESIZE_NWSE:
            return wl_cursor_theme_get_cursor(theme, "nwse-resize");
    }
    return nullptr;
}
