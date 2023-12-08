#include "WindowManagerWayland.hh"
#include "WindowWayland.hh"
#include <cstdio>
#include <limits>
#include <impl/Library.hh>
#include <impl/JNILocal.hh>
#include "AppWayland.hh"
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include "KeyWayland.hh"
#include "MouseButtonWayland.hh"
#include "StringUTF16.hh"
#include <algorithm>
#include <system_error>
#include "Log.hh"
#include <cstring>
#include <cerrno>
#include "Output.hh"
#include <libdecor-0/libdecor.h>
#include <linux/input-event-codes.h>
#include <wayland-util.h>
#include <xkbcommon/xkbcommon.h>
#include <sys/mman.h>

using namespace jwm;

wl_registry_listener WindowManagerWayland::_registryListener = {
    .global = WindowManagerWayland::registryHandleGlobal,
    .global_remove = WindowManagerWayland::registryHandleGlobalRemove
};
wl_pointer_listener WindowManagerWayland::_pointerListener = {
  .enter = WindowManagerWayland::pointerHandleEnter,
  .leave = WindowManagerWayland::pointerHandleLeave,
  .motion = WindowManagerWayland::pointerHandleMotion,
  .button = WindowManagerWayland::pointerHandleButton,
  .axis = WindowManagerWayland::pointerHandleAxis
};

libdecor_interface WindowManagerWayland::_decorInterface = {
    .error = WindowManagerWayland::libdecorError
};
wl_keyboard_listener WindowManagerWayland::_keyboardListener = {
    .keymap = WindowManagerWayland::keyboardKeymap,
    .enter = WindowManagerWayland::keyboardEnter,
    .leave = WindowManagerWayland::keyboardLeave,
    .key = WindowManagerWayland::keyboardKey,
    .modifiers = WindowManagerWayland::keyboardModifiers,
    .repeat_info = WindowManagerWayland::keyboardRepeatInfo
};
wl_seat_listener WindowManagerWayland::_seatListener = {
    .capabilities = WindowManagerWayland::seatCapabilities,
    .name = WindowManagerWayland::seatName
};
WindowManagerWayland::WindowManagerWayland():
    display(wl_display_connect(nullptr)) {
        registry = wl_display_get_registry(display);
        wl_registry_add_listener(registry, &_registryListener, this);

        wl_display_roundtrip(display);
        


        if (!(shm && compositor && deviceManager && seat)) {
            // ???
            // Bad. Means our compositor no supportie : (
            throw std::system_error(ENOTSUP, std::generic_category(), "Unsupported compositor");
        }
       
        // ???: Moving this after libdecor_new causes input to not work
        wl_seat_add_listener(seat, &_seatListener, this);

        decorCtx = libdecor_new(display, &_decorInterface);


        wl_display_roundtrip(display);
        {
            wl_cursor_theme* cursor_theme = wl_cursor_theme_load(nullptr, 24, shm);
            // TODO: what about if missing : (
            auto loadCursor = [&](const char* name) {
                wl_cursor* cursor = wl_cursor_theme_get_cursor(cursor_theme, name);
                wl_cursor_image* cursorImage = cursor->images[0];
                return cursorImage;
            };

            _cursors[static_cast<int>(jwm::MouseCursor::ARROW         )] = loadCursor("default");
            _cursors[static_cast<int>(jwm::MouseCursor::CROSSHAIR     )] = loadCursor("crosshair");
            _cursors[static_cast<int>(jwm::MouseCursor::HELP          )] = loadCursor("help");
            _cursors[static_cast<int>(jwm::MouseCursor::POINTING_HAND )] = loadCursor("pointer");
            _cursors[static_cast<int>(jwm::MouseCursor::IBEAM         )] = loadCursor("text");
            _cursors[static_cast<int>(jwm::MouseCursor::NOT_ALLOWED   )] = loadCursor("not-allowed");
            _cursors[static_cast<int>(jwm::MouseCursor::WAIT          )] = loadCursor("watch");
            _cursors[static_cast<int>(jwm::MouseCursor::RESIZE_NS     )] = loadCursor("ns-resize");
            _cursors[static_cast<int>(jwm::MouseCursor::RESIZE_WE     )] = loadCursor("ew-resize");
            _cursors[static_cast<int>(jwm::MouseCursor::RESIZE_NESW   )] = loadCursor("nesw-resize");
            _cursors[static_cast<int>(jwm::MouseCursor::RESIZE_NWSE   )] = loadCursor("nwse-resize");
            
            cursorSurface = wl_compositor_create_surface(compositor);

            wl_surface_attach(cursorSurface, 
                    wl_cursor_image_get_buffer(_cursors[static_cast<int>(jwm::MouseCursor::ARROW)]), 0, 0);
            wl_surface_commit(cursorSurface);
        }


}




void WindowManagerWayland::runLoop() {
    _runLoop = true;
    int pipes[2];

    char buf[100];
    if (pipe(pipes)) {
        printf("failed to open pipe\n");
        return;
    }
    notifyFD = pipes[1];
    fcntl(pipes[1], F_SETFL, O_NONBLOCK); // notifyLoop no blockie : )
    struct pollfd wayland_out = {.fd=wl_display_get_fd(display),.events=POLLOUT};
    struct pollfd ps[] = {
        {.fd=libdecor_get_fd(decorCtx), .events=POLLIN}, 
        {.fd=pipes[0], .events=POLLIN},
    };
    // who be out here running they loop
    while (_runLoop) {
        if (jwm::classes::Throwable::exceptionThrown(app.getJniEnv()))
            _runLoop = false;
        _processCallbacks();
        // block until event : )
        if (poll(&ps[0], 2, -1) < 0) {
            printf("error with pipe\n");
            break;
        }
        if (ps[1].revents & POLLIN) {
            while (read(pipes[0], buf, sizeof(buf)) == sizeof(buf)) { }
        }
        if (ps[0].revents & POLLIN) {
            libdecor_dispatch(decorCtx, -1);
        }
        notifyBool.store(false);
    }

    notifyFD = -1;
    close(pipes[0]);
    close(pipes[1]);
    
}

void WindowManagerWayland::libdecorError(libdecor* context, enum libdecor_error error, const char* message) {
    // ???
    fprintf(stderr, "Caught error (%d): %s\n", error, message);
    throw std::runtime_error("lib decor error > : (");
}
void WindowManagerWayland::_processCallbacks() {
    {
        // process ui thread callbacks
        std::unique_lock<std::mutex> lock(_taskQueueLock);

        while (!_taskQueue.empty()) {
            auto callback = std::move(_taskQueue.front());
            _taskQueue.pop();
            lock.unlock();
            callback();
            lock.lock();
        }        
    }
    {
        // copy window list in case one closes any other, invalidating some iterator in _nativeWindowToMy
        std::vector<WindowWayland*> copy;
        for (auto& p : _nativeWindowToMy) {
            copy.push_back(p.second);
        }
        // process redraw requests
        for (auto p : copy) {
            if (p->isRedrawRequested()) {
                p->unsetRedrawRequest();
                if (p->_layer && p->_visible) {
                    p->_layer->makeCurrent();
                }
                p->dispatch(classes::EventFrame::kInstance);
            }
        }
    }
}

void WindowManagerWayland::registryHandleGlobal(void* data, wl_registry *registry,
        uint32_t name, const char* interface, uint32_t version) {
    WindowManagerWayland* self = reinterpret_cast<WindowManagerWayland*>(data);
    if (strcmp(interface, wl_compositor_interface.name) == 0) {
        // EGL apparently requires at least a version of 4 here : )
        self->compositor = (wl_compositor*)wl_registry_bind(registry, name,
                &wl_compositor_interface, 4);
    } else if (strcmp(interface, wl_shm_interface.name) == 0) {
        self->shm = (wl_shm*)wl_registry_bind(registry, name,
                &wl_shm_interface, 1);
    } else if (strcmp(interface, wl_data_device_manager_interface.name) == 0) {
        self->deviceManager = (wl_data_device_manager*)wl_registry_bind(registry, name,
                &wl_data_device_manager_interface, 1);
    } else if (strcmp(interface,  wl_seat_interface.name) == 0) {
        self->seat = (wl_seat*)wl_registry_bind(registry, name,
                &wl_seat_interface, 1);
    } else if (strcmp(interface, wl_output_interface.name) == 0) {
        wl_output* output = (wl_output*)wl_registry_bind(registry, name,
                &wl_output_interface, 2);
        Output* good = new Output(output, name);
        self->outputs.push_back(good);
    }
}
void WindowManagerWayland::registryHandleGlobalRemove(void* data, wl_registry *registry, uint32_t name) {
    auto self = reinterpret_cast<WindowManagerWayland*>(data);
    for (std::list<Output*>::iterator it = self->outputs.begin(); it != self->outputs.end();) {
        if ((*it)->_name == name) {
            self->outputs.erase(it);
            break;
        }
        ++it;
    }
}
WindowWayland* WindowManagerWayland::getWindowForNative(wl_surface* surface) {
    // the tag makes it safe. Should:TM: be faster than searching a list every time
    const char* const* tag = wl_proxy_get_tag((wl_proxy*) surface);
    if (tag != &WindowWayland::_windowTag) {
        return nullptr;
    }
    return reinterpret_cast<WindowWayland*>(wl_surface_get_user_data(surface));
    /*
    WindowWayland* myWindow = nullptr;
    auto it = _nativeWindowToMy.find(surface);
    if (it != _nativeWindowToMy.end())
        myWindow = it->second;
    return myWindow;
    */
}
void WindowManagerWayland::pointerHandleEnter(void* data, wl_pointer* pointer, uint32_t serial,
        wl_surface *surface, wl_fixed_t surface_x, wl_fixed_t surface_y) {
    WindowManagerWayland* self = (WindowManagerWayland*)data;
    wl_cursor_image* image = self->_cursors[static_cast<int>(jwm::MouseCursor::ARROW)];
    wl_pointer_set_cursor(pointer, serial, self->cursorSurface,  image->hotspot_x, image->hotspot_y);
    if (self->getWindowForNative(surface))
        self->focusedSurface = surface;
}
void WindowManagerWayland::pointerHandleLeave(void* data, wl_pointer* pointer, uint32_t serial,
        wl_surface *surface) {
    WindowManagerWayland* self = (WindowManagerWayland*)data;
    self->focusedSurface = nullptr;
    // ???
    self->mouseMask = 0;
}
void WindowManagerWayland::pointerHandleMotion(void* data, wl_pointer* pointer,
        uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y) {
    WindowManagerWayland* self = (WindowManagerWayland*)data;
    if (self->focusedSurface) {
        ::WindowWayland* window = self->getWindowForNative(self->focusedSurface);
        // God is dead if window is null
        if (window)
            self->mouseUpdate(window, 
                    wl_fixed_to_int(surface_x) * window->_scale, 
                    wl_fixed_to_int(surface_y) * window->_scale, self->mouseMask);
    }
    
}
void WindowManagerWayland::pointerHandleButton(void* data, wl_pointer* pointer, 
        uint32_t serial, uint32_t time, uint32_t button, uint32_t state) {
    using namespace classes;
    WindowManagerWayland* self = (WindowManagerWayland*)data;
    if (!self->focusedSurface) return;
    if (state == 0) {
        // release
        switch (button) {
            // primary
            case BTN_LEFT:
                self->mouseMask &= ~0x100;
                break;
            // secondary
            case BTN_RIGHT:
                self->mouseMask &= ~0x400;
                break;
            // middle
            case BTN_MIDDLE:
                self->mouseMask &= ~0x200;
                break;
            default:
                break;
        }
        
        if (MouseButtonWayland::isButton(button) && self->focusedSurface) {
            jwm::JNILocal<jobject> eventButton(
                    app.getJniEnv(),
                    EventMouseButton::make(
                            app.getJniEnv(),
                            MouseButtonWayland::fromNative(button),
                            false,
                            self->lastMousePosX,
                            self->lastMousePosY,
                            jwm::KeyWayland::getModifiers(self->_xkbState)
                        )
                    );
            WindowWayland* window = self->getWindowForNative(self->focusedSurface);
            if (window)
                window->dispatch(eventButton.get());
        }
    } else {
        // down
        switch (button) {
            // primary
            case BTN_LEFT:
                self->mouseMask |= 0x100;
                break;
            // secondary
            case BTN_RIGHT:
                self->mouseMask |= 0x400;
                break;
            // middle
            case BTN_MIDDLE:
                self->mouseMask |= 0x200;
                break;
            default:
                break;
        }
        
        if (MouseButtonWayland::isButton(button) && self->focusedSurface) {
            jwm::JNILocal<jobject> eventButton(
                    app.getJniEnv(),
                    EventMouseButton::make(
                            app.getJniEnv(),
                            MouseButtonWayland::fromNative(button),
                            true,
                            self->lastMousePosX,
                            self->lastMousePosY,
                            jwm::KeyWayland::getModifiers(self->_xkbState)
                        )
                    );
            // me when this stuff is NULL : (
            WindowWayland* window = self->getWindowForNative(self->focusedSurface);
            if (window)
                window->dispatch(eventButton.get());
        }
    }
}
void WindowManagerWayland::pointerHandleAxis(void* data, wl_pointer* pointer, 
        uint32_t time, uint32_t axis, wl_fixed_t value) {}
void WindowManagerWayland::keyboardKeymap(void* data, wl_keyboard* keyboard, uint32_t format,
        int32_t fd, uint32_t size) {
    auto self = reinterpret_cast<WindowManagerWayland*>(data);

    if (format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1) {
        close(fd);
        fprintf(stderr, "no xkb keymap\n");
        return;
    }

    char* map_str = reinterpret_cast<char*>(mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0));
    if (map_str == MAP_FAILED) {
        close(fd);
        fprintf(stderr, "keymap mmap failed: %s", strerror(errno));
        return;
    }

    xkb_keymap* keymap = xkb_keymap_new_from_string(
                self->_xkbContext, map_str,
                XKB_KEYMAP_FORMAT_TEXT_V1,
                XKB_KEYMAP_COMPILE_NO_FLAGS
            );
    munmap(map_str, size);
    close(fd);

    if (!keymap) {
        return;
    }
    self->_xkbState = xkb_state_new(keymap);

    xkb_keymap_unref(keymap);
}
void WindowManagerWayland::keyboardEnter(void* data, wl_keyboard* keyboard, uint32_t serial, wl_surface* surface,
        wl_array *keys) {
    auto self = reinterpret_cast<WindowManagerWayland*>(data);
    self->keyboardFocus = surface;
}
void WindowManagerWayland::keyboardLeave(void* data, wl_keyboard* keyboard, uint32_t serial, wl_surface* surface) {
    auto self = reinterpret_cast<WindowManagerWayland*>(data);
    self->keyboardFocus = nullptr;
}

void WindowManagerWayland::keyboardKey(void* data, wl_keyboard* keyboard, uint32_t serial, uint32_t time,
        uint32_t key, uint32_t state)
{
    auto self = reinterpret_cast<WindowManagerWayland*>(data);
    if (!self->_xkbState) return;
    const xkb_keysym_t *syms;

    if (xkb_state_key_get_syms(self->_xkbState, key + 8, &syms) != 1)
        return;
    jwm::Key jwmKey = KeyWayland::fromNative(syms[0]);
    // TODO: while unlikely, it could be possible that you can be entering text even if the 
    // pointer hasn't entered
    if (self->keyboardFocus && jwmKey != jwm::Key::UNDEFINED) {
        jwm::KeyLocation location;
        JNILocal<jobject> keyEvent(
            app.getJniEnv(),
            classes::EventKey::make(
                    app.getJniEnv(),
                    jwmKey,
                    state == WL_KEYBOARD_KEY_STATE_PRESSED,
                    KeyWayland::getModifiers(self->_xkbState),
                    location
                )
            );
        auto window = self->getWindowForNative(self->keyboardFocus);
        if (window)
            window->dispatch(keyEvent.get());
    }
    if (state != WL_KEYBOARD_KEY_STATE_PRESSED) return;

    char textBuffer[0x40];
    int count = xkb_state_key_get_utf8(self->_xkbState, key + 8, textBuffer, sizeof(textBuffer)-1);
    // ???
    if (count >= sizeof(textBuffer) - 1) {
        return;
    }
    if (count > 0) {
        // ignore sinful control symbols
        if (textBuffer[0] != 127 && textBuffer[0] > 0x1f) {
            JNIEnv* env = app.getJniEnv();

            jwm::StringUTF16 converted = reinterpret_cast<const char*>(textBuffer);
            jwm::JNILocal<jstring> jtext = converted.toJString(env);

            jwm::JNILocal<jobject> eventTextInput(env, classes::EventTextInput::make(env, jtext.get()));
            

            auto window = self->getWindowForNative(self->keyboardFocus);
            if (window)
                window->dispatch(eventTextInput.get());
        }
    }

}
void WindowManagerWayland::keyboardModifiers(void* data, wl_keyboard* keyboard,
        uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked,
        uint32_t group) {
    auto self = reinterpret_cast<WindowManagerWayland*>(data);
    if (!self->_xkbState) return;
    xkb_state_update_mask(self->_xkbState,
            mods_depressed, mods_latched, mods_locked,
            0, 0, group);
}

void WindowManagerWayland::keyboardRepeatInfo(void* data, wl_keyboard* keyboard,
        int32_t rate, int32_t delay) {
    // TODO: The client (for some godforsaken reason) is expected to handle repeating characters
}

void WindowManagerWayland::seatCapabilities(void* data, wl_seat* seat, uint32_t capabilities) {
    auto self = reinterpret_cast<WindowManagerWayland*>(data);
    if ((capabilities & WL_SEAT_CAPABILITY_POINTER) &&
            !self->pointer) {
        self->pointer = wl_seat_get_pointer(seat);
        wl_pointer_add_listener(self->pointer, &_pointerListener, self);
    } else if (!(capabilities & WL_SEAT_CAPABILITY_POINTER) && self->pointer) {
        wl_pointer_release(self->pointer);
        self->pointer = nullptr;
    }

    if ((capabilities & WL_SEAT_CAPABILITY_KEYBOARD) &&
            !self->keyboard) {
        self->keyboard = wl_seat_get_keyboard(seat);
        self->_xkbContext = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
        wl_keyboard_add_listener(self->keyboard, &_keyboardListener, self);
    } else if (!(capabilities & WL_SEAT_CAPABILITY_KEYBOARD) &&
            self->keyboard) {
        xkb_context_unref(self->_xkbContext);
        wl_keyboard_release(self->keyboard);
        self->keyboard = nullptr;
    }
}
void WindowManagerWayland::seatName(void* data, wl_seat* seat, const char* name) {
    // who cares
}
std::vector<std::string> WindowManagerWayland::getClipboardFormats() {
    /*
    XConvertSelection(display,
                      _atoms.CLIPBOARD,
                      _atoms.TARGETS,
                      _atoms.JWM_CLIPBOARD,
                      nativeHandle,
                      CurrentTime);

    XEvent ev;

    // fetch mime types
    std::vector<std::string> result;
    
    // using lambda here in order to break 2 loops
    [&]{
        while (_runLoop) {
            while (XPending(display)) {
                XNextEvent(display, &ev);
                if (ev.type == SelectionNotify) {
                    int format;
                    unsigned long count, lengthInBytes;
                    Atom type;
                    Atom* properties;
                    XGetWindowProperty(display, nativeHandle, _atoms.JWM_CLIPBOARD, 0, 1024 * sizeof(Atom), false, XA_ATOM, &type, &format, &count, &lengthInBytes, reinterpret_cast<unsigned char**>(&properties));
                    
                    for (unsigned long i = 0; i < count; ++i) {
                        char* str = XGetAtomName(display, properties[i]);
                        if (str) {
                            std::string s = str;
                            // include only mime types
                            if (s.find('/') != std::string::npos) {
                                result.push_back(s);
                            } else if (s == "UTF8_STRING") {
                                // HACK: treat UTF8_STRING as text/plain under the hood
                                // avoid duplicates
                                std::string textPlain = "text/plain";
                                if (std::find(result.begin(), result.end(), textPlain) != result.end()) {
                                    result.push_back(textPlain);
                                }
                            }
                            XFree(str);
                        }
                    }
                    
                    XFree(properties);
                    return;
                } else {
                    _processXEvent(ev);
                }
            
            }
            _processCallbacks();
        }
    }();

    // fetching data

    XDeleteProperty(display, nativeHandle, _atoms.JWM_CLIPBOARD);
    */
    std::vector<std::string> result;
    return result;
}
void WindowManagerWayland::mouseUpdate(WindowWayland* myWindow, uint32_t x, uint32_t y, uint32_t mask) {
    using namespace classes;
    if (!myWindow)
        return;
    // impl me : )
    if (lastMousePosX == x && lastMousePosY == y) return;
    lastMousePosX = x;
    lastMousePosY = y;
    int movementX = 0, movementY = 0;
    jwm::JNILocal<jobject> eventMove(
        app.getJniEnv(),
        EventMouseMove::make(app.getJniEnv(),
            x,
            y,
            movementX,
            movementY,
            jwm::MouseButtonWayland::fromNativeMask(mask),
            // impl me!
            jwm::KeyWayland::getModifiers(_xkbState)
            )
        );
    auto foo = eventMove.get();
    myWindow->dispatch(foo);
}
jwm::ByteBuf WindowManagerWayland::getClipboardContents(const std::string& type) {
    auto nativeHandle = _nativeWindowToMy.begin()->first;
    return {};
}

void WindowManagerWayland::registerWindow(WindowWayland* window) {
    _nativeWindowToMy[window->_waylandWindow] = window;
}

void WindowManagerWayland::unregisterWindow(WindowWayland* window) {
    auto it = _nativeWindowToMy.find(window->_waylandWindow);
    if (it != _nativeWindowToMy.end()) {
        _nativeWindowToMy.erase(it);
    }
}

void WindowManagerWayland::terminate() {
    _runLoop = false;
}

void WindowManagerWayland::setClipboardContents(std::map<std::string, ByteBuf>&& c) {
    assert(("create at least one window in order to use clipboard" && !_nativeWindowToMy.empty()));
    _myClipboardContents = c;
    // impl me : )
    auto window = _nativeWindowToMy.begin()->first;
    // XSetSelectionOwner(display, XA_PRIMARY, window, CurrentTime);
    // XSetSelectionOwner(display, _atoms.CLIPBOARD, window, CurrentTime);
}

void WindowManagerWayland::enqueueTask(const std::function<void()>& task) {
    std::unique_lock<std::mutex> lock(_taskQueueLock);
    _taskQueue.push(task);
    _taskQueueNotify.notify_one();
    notifyLoop();
}

void WindowManagerWayland::notifyLoop() {
    // maybe just do nothing?
    if (notifyFD==-1) return;
    // fast notifyBool path to not make system calls when not necessary
    if (!notifyBool.exchange(true)) {
        char dummy[1] = {0};
        int unused = write(notifyFD, dummy, 1); // this really shouldn't fail, but if it does, the pipe should either be full (good), or dead (bad, but not our business)
    }
}
