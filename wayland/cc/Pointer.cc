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
    if (!Pointer::ownPointer(pointer)) {
        return;
    }
    auto self = reinterpret_cast<Pointer*>(data);
    if (auto window = self->_wm.getWindowForNative(surface)) {
        self->_serial = serial;
        self->_focusedSurface = jwm::ref(window);
        window->setCursor(jwm::MouseCursor::ARROW);
        // frame probably isn't called so I immediately call
        self->mouseUpdateUnscaled(wl_fixed_to_int(surface_x), wl_fixed_to_int(surface_y), 0, 0, self->_mouseMask);
    }
}

static void pointerLeave(void* data, wl_pointer* pointer, uint32_t serial,
        wl_surface* surface) 
{
    if (!Pointer::ownPointer(pointer)) {
        return;
    }
    auto self = reinterpret_cast<Pointer*>(data);
    if (self->_focusedSurface && self->_focusedSurface->isNativeSelf(surface)) { 
        jwm::unref(&self->_focusedSurface);
        self->_mouseMask = 0;
        self->_serial = 0;
    }
}
static void pointerMotion(void* data, wl_pointer* pointer, uint32_t time, 
        wl_fixed_t surface_x, wl_fixed_t surface_y) 
{
    if (!Pointer::ownPointer(pointer)) {
        return;
    }
    auto self = reinterpret_cast<Pointer*>(data);
    self->unhide();
    self->_absX = wl_fixed_to_int(surface_x);
    self->_absY = wl_fixed_to_int(surface_y);
    self->_movement = true;
    // I only unhide here, bc i'm unsure of what exactly is wanted with mouse lock.
    // This event isn't sent on mouse lock, only relative events are sent.
    self->unhide();
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
                            self->_absX,
                            self->_absY,
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
                            self->_absX,
                            self->_absY,
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
            self->_dY += -fvalue;
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
                            self->_absX * win->_scale,
                            self->_absY * win->_scale,
                            jwm::KeyWayland::getModifiers(self->_wm.getXkbState())
                        )
                );
        win->dispatch(eventAxis.get());

        self->_dX = 0.0f;
        self->_dY = 0.0f;
    }
    if (self->_movement || self->_dXPos != 0.0 || self->_dYPos != 0.0) {
        auto scale = win->_scale;
        self->mouseUpdateUnscaled(self->_absX * scale, self->_absY * scale, static_cast<int>(self->_dXPos * scale), static_cast<int>(self->_dYPos * scale), self->_mouseMask);
        self->_movement = false;
        self->_dXPos = 0.0;
        self->_dYPos = 0.0;
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

static void relativePointerRelativeMotion(void* data, zwp_relative_pointer_v1* relative, uint32_t utime_hi, uint32_t utime_lo, 
        wl_fixed_t dx, wl_fixed_t dy, wl_fixed_t dx_unaccel, wl_fixed_t dy_unaccel) {
    auto self = reinterpret_cast<Pointer*>(data);

    self->_dXPos += static_cast<float>(wl_fixed_to_double(dx));
    self->_dYPos += static_cast<float>(wl_fixed_to_double(dy));
}
static zwp_relative_pointer_v1_listener relativePointerListener = {
    .relative_motion = relativePointerRelativeMotion
};
Pointer::Pointer(wl_pointer* pointer, WindowManagerWayland* wm):
    _pointer(pointer),
    _wm(*wm) 
{
    _surface = wl_compositor_create_surface(_wm.compositor);
    wl_pointer_add_listener(pointer, &_pointerListener, this);
    wl_proxy_set_tag((wl_proxy*)pointer, &AppWayland::proxyTag);
    if (_wm.relativePointerManager) {
        _relative = zwp_relative_pointer_manager_v1_get_relative_pointer(_wm.relativePointerManager, pointer);
        zwp_relative_pointer_v1_add_listener(_relative, &relativePointerListener, this);
    }
}

Pointer::~Pointer()
{
    if (_pointer)
        wl_pointer_release(_pointer);
    if (_surface)
        wl_surface_destroy(_surface);
}

void Pointer::mouseUpdate(uint32_t x, uint32_t y, int32_t relX, int32_t relY, uint32_t mask) {
    auto window = _focusedSurface;
    if (!window)
        return;

    jwm::JNILocal<jobject> eventMove(
        app.getJniEnv(),
        jwm::classes::EventMouseMove::make(app.getJniEnv(),
            x,
            y,
            relX,
            relY,
            jwm::MouseButtonWayland::fromNativeMask(mask),
            // impl me!
            jwm::KeyWayland::getModifiers(_wm.getXkbState())
            )
        );
    window->dispatch(eventMove.get());
}

void Pointer::mouseUpdateUnscaled(uint32_t x, uint32_t y, int32_t relX, int32_t relY, uint32_t mask) {
    if (!_focusedSurface) return;
    auto newX = x * _focusedSurface->_scale;
    auto newY = y * _focusedSurface->_scale;
    auto newDX = relX * _focusedSurface->_scale;
    auto newDY = relY * _focusedSurface->_scale;

    mouseUpdate(newX, newY, newDX, newDY, mask);
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

bool Pointer::ownPointer(wl_pointer* pointer) {
    return AppWayland::ownProxy((wl_proxy*) pointer);
}

static void lockLocked(void* data, zwp_locked_pointer_v1* pointer) {
    auto self = reinterpret_cast<Pointer*>(data);

    self->_locked = true;
}
static void lockUnlocked(void* data, zwp_locked_pointer_v1* pointer) {
    zwp_locked_pointer_v1_destroy(pointer);

    auto self = reinterpret_cast<Pointer*>(data);

    self->_lock = nullptr;
    if (self->_locked) {
        // Request a new lock on surface reenter
        self->lock();
    }
}

static zwp_locked_pointer_v1_listener lockListener = {
    .locked = lockLocked,
    .unlocked = lockUnlocked
};
void Pointer::lock() {
    if (_wm.pointerConstraints && _focusedSurface && _focusedSurface->_waylandWindow) {
        if (_lock) {
            zwp_locked_pointer_v1_destroy(_lock);
        }
        _lock = zwp_pointer_constraints_v1_lock_pointer(_wm.pointerConstraints, _focusedSurface->_waylandWindow, 
                _pointer, nullptr, ZWP_POINTER_CONSTRAINTS_V1_LIFETIME_ONESHOT);
        zwp_locked_pointer_v1_add_listener(_lock, &lockListener, this);
        hide();
    }
}
void Pointer::unlock() {
    if (_lock) {
        zwp_locked_pointer_v1_destroy(_lock);
    }
    unhide();
    _lock = nullptr;
    _locked = false;
}

void Pointer::hide() {
    if (_hidden) return;
    _hidden = true;
    if (_surface) {
        wl_surface_attach(_surface, nullptr, 0, 0);
        wl_surface_commit(_surface);
    }
}

void Pointer::unhide() {
    if (!_hidden) return;
    _hidden = false;
    setCursor(_scale, _cursor);
}

void Pointer::setCursor(int scale, jwm::MouseCursor cursor) {
    _cursor = cursor;
    _scale = scale;
    auto wayCursor = getCursorFor(scale, cursor)->images[0];
    auto buf = wl_cursor_image_get_buffer(wayCursor);
    wl_surface_attach(_surface, buf, 0, 0);
    wl_surface_set_buffer_scale(_surface, scale);
    wl_surface_damage_buffer(_surface, 0, 0, INT32_MAX, INT32_MAX);
    updateHotspot(wayCursor->hotspot_x, wayCursor->hotspot_y);
    wl_surface_commit(_surface);
}
