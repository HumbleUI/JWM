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
#include <algorithm>
#include <chrono>

using namespace jwm;

wl_registry_listener WindowManagerWayland::_registryListener = {
    .global = WindowManagerWayland::registryHandleGlobal,
    .global_remove = WindowManagerWayland::registryHandleGlobalRemove
};
static void pointerFrame(void* data, wl_pointer* pointer) {
    auto self = reinterpret_cast<WindowManagerWayland*>(data);
    if (!self->getFocusedSurface()) return;
    if (self->_dX != 0.0f || self->_dY != 0.0f) {
        auto env = app.getJniEnv();
        auto win = self->getFocusedSurface();
        jwm::JNILocal<jobject> eventAxis(
                    env,
                    jwm::classes::EventMouseScroll::make(
                            env,
                            self->_dX * win->_scale,
                            self->_dY * win->_scale,
                            0.0f,
                            0.0f,
                            0.0f,
                            self->lastMousePosX,
                            self->lastMousePosY,
                            jwm::KeyWayland::getModifiers(self->getXkbState())
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
// Lambdas turn into null pointers at runtime. God knows why.
wl_pointer_listener WindowManagerWayland::_pointerListener = {
  .enter = WindowManagerWayland::pointerHandleEnter,
  .leave = WindowManagerWayland::pointerHandleLeave,
  .motion = WindowManagerWayland::pointerHandleMotion,
  .button = WindowManagerWayland::pointerHandleButton,
  .axis = WindowManagerWayland::pointerHandleAxis,
  .frame = pointerFrame,
  .axis_source = pointerAxisSource,
  .axis_stop = pointerAxisStop,
  .axis_discrete = pointerAxisDiscrete
};

libdecor_interface WindowManagerWayland::_decorInterface = {
    .error = WindowManagerWayland::libdecorError
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
        dataDevice = wl_data_device_manager_get_data_device(deviceManager, seat);
        wl_data_device_add_listener(dataDevice, &_deviceListener, this);
        decorCtx = libdecor_new(display, &_decorInterface);


        wl_display_roundtrip(display);
        cursorSurface = wl_compositor_create_surface(compositor);

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
    struct pollfd ps[] = {
        {.fd=libdecor_get_fd(decorCtx), .events=POLLIN}, 
        {.fd=pipes[0], .events=POLLIN},
    };

    // who be out here running they loop
    while (_runLoop) {
        if (jwm::classes::Throwable::exceptionThrown(app.getJniEnv()))
            _runLoop = false;

        // block until event : )
        int timeout = -1;
        if (_keyboard && _keyboard->_repeating && _keyboard->getFocus()) {
            if (_keyboard->_repeatRate > 0) {
                auto now = std::chrono::steady_clock::now();
                auto target = _keyboard->_nextRepeat;
                if (now < target) {
                    timeout = std::chrono::duration_cast<std::chrono::milliseconds>(target - now).count();
                } else {
                    _processKeyboard();
                }
            }
        }
        if (poll(&ps[0], 2, timeout) < 0) {
            printf("error with pipe\n");
            break;
        }
        if (ps[0].revents & POLLIN) {
            if (libdecor_dispatch(decorCtx, -1) < 0) {
                fprintf(stderr, "error with dispatch\n");
                break;
            }
        }

        if (ps[1].revents & POLLIN) {
            while (read(pipes[0], buf, sizeof(buf)) == sizeof(buf)) { }
        }
        if (ps[0].revents & POLLIN || ps[1].revents & POLLIN) {
            _processCallbacks();
        } else {
            // don't test if we already calculated earlier
            _processKeyboard();
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
                if (p->_visible && p->_configured) {
                    if (p->_layer) {
                        p->_layer->makeCurrent();
                    }
                    p->dispatch(classes::EventFrame::kInstance);
                }
            }
        }
    }
   
}
void WindowManagerWayland::_processKeyboard() {
    if (_keyboard && _keyboard->_repeating && _keyboard->getFocus()) {
        auto now = std::chrono::steady_clock::now();
        _keyboard->_nextRepeat = now + std::chrono::milliseconds(_keyboard->_repeatRate);
        auto focus = _keyboard->getFocus();
        auto env = jwm::app.getJniEnv();
        jwm::KeyLocation location = jwm::KeyLocation::DEFAULT;
        JNILocal<jobject> keyOffEvent(
            env,
            classes::EventKey::make(
                    env,
                    _keyboard->_repeatKey,
                    false,
                    KeyWayland::getModifiers(_keyboard->_state),
                    location
                )
            );

        JNILocal<jobject> keyEvent(
            env,
            classes::EventKey::make(
                    env,
                    _keyboard->_repeatKey,
                    true,
                    KeyWayland::getModifiers(_keyboard->_state),
                    location
                )
            );
        focus->dispatch(keyOffEvent.get());
        focus->dispatch(keyEvent.get());
        if (_keyboard->_repeatingText) {
            jwm::JNILocal<jstring> jtext = _keyboard->_repeatText.toJString(env);
            jwm::JNILocal<jobject> eventTextInput(env, classes::EventTextInput::make(env, jtext.get()));

            focus->dispatch(eventTextInput.get());
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
                &wl_seat_interface, 5);
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
    self->mouseSerial = serial;
    if (auto window = self->getWindowForNative(surface)) {
        window->setCursor(jwm::MouseCursor::ARROW);
        self->setFocusedSurface(window);
    }
}
void WindowManagerWayland::pointerHandleLeave(void* data, wl_pointer* pointer, uint32_t serial,
        wl_surface *surface) {
    WindowManagerWayland* self = (WindowManagerWayland*)data;
    self->setFocusedSurface(nullptr);
    // ???
    self->mouseMask = 0;
    self->mouseSerial = -1;
}
void WindowManagerWayland::pointerHandleMotion(void* data, wl_pointer* pointer,
        uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y) {
    WindowManagerWayland* self = (WindowManagerWayland*)data;
    if (self->getFocusedSurface()) {
        ::WindowWayland* window = self->getFocusedSurface();
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
    if (!self->getFocusedSurface()) return;
    auto window = self->getFocusedSurface();
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
        
        if (MouseButtonWayland::isButton(button)) {
            jwm::JNILocal<jobject> eventButton(
                    app.getJniEnv(),
                    EventMouseButton::make(
                            app.getJniEnv(),
                            MouseButtonWayland::fromNative(button),
                            false,
                            self->lastMousePosX,
                            self->lastMousePosY,
                            jwm::KeyWayland::getModifiers(self->getXkbState())
                        )
                    );
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
        
        if (MouseButtonWayland::isButton(button)) {
            jwm::JNILocal<jobject> eventButton(
                    app.getJniEnv(),
                    EventMouseButton::make(
                            app.getJniEnv(),
                            MouseButtonWayland::fromNative(button),
                            true,
                            self->lastMousePosX,
                            self->lastMousePosY,
                            jwm::KeyWayland::getModifiers(self->getXkbState())
                        )
                    );
            window->dispatch(eventButton.get());
        }
    }
}
void WindowManagerWayland::pointerHandleAxis(void* data, wl_pointer* pointer, 
        uint32_t time, uint32_t axis, wl_fixed_t value) {
    auto self = reinterpret_cast<WindowManagerWayland*>(data);
    if (!self->getFocusedSurface()) return;
    switch (axis) {
        case WL_POINTER_AXIS_VERTICAL_SCROLL:
            self->_dY += static_cast<float>(wl_fixed_to_double(value));
            break;
        case WL_POINTER_AXIS_HORIZONTAL_SCROLL:
            self->_dX += static_cast<float>(wl_fixed_to_double(value));
            break;
        default:
            break;
    }
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
            !self->_keyboard) {
        self->_keyboard = new Keyboard(wl_seat_get_keyboard(seat), self);
    } else if (!(capabilities & WL_SEAT_CAPABILITY_KEYBOARD) &&
            self->_keyboard) {
        self->_keyboard = nullptr;
    }
}
void WindowManagerWayland::seatName(void* data, wl_seat* seat, const char* name) {
    // who cares
}
static void offerOffer(void* data, wl_data_offer* offer, const char* mimeType) {
    auto self = reinterpret_cast<WindowManagerWayland*>(data);
    self->_currentMimeTypes.push_back(std::string(mimeType));
}
wl_data_offer_listener WindowManagerWayland::_offerListener = {
    .offer = offerOffer
};
static void deviceDataOffer(void* data, wl_data_device* device, wl_data_offer* offer) {
    auto self = reinterpret_cast<WindowManagerWayland*>(data);
    self->_currentMimeTypes = {};
    wl_data_offer_add_listener(offer, &WindowManagerWayland::_offerListener, data);
}
static void deviceSelection(void* data, wl_data_device* device, wl_data_offer* offer) {
    auto self = reinterpret_cast<WindowManagerWayland*>(data);
    // if null then w/e
    self->currentOffer = offer;
}
wl_data_device_listener WindowManagerWayland::_deviceListener = {
    .data_offer = deviceDataOffer,
    .selection = deviceSelection

};
std::vector<std::string> WindowManagerWayland::getClipboardFormats() {
    return { _currentMimeTypes.begin(), _currentMimeTypes.end()};
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
            jwm::KeyWayland::getModifiers(getXkbState())
            )
        );
    auto foo = eventMove.get();
    myWindow->dispatch(foo);
}
jwm::ByteBuf WindowManagerWayland::getClipboardContents(const std::string& type) {
    auto it = _myClipboardContents.find(type);
    if (it != _myClipboardContents.end()) {
        return it->second;
    } else if (currentSource) {
        // Self paste
        auto it2 = _myClipboardSource.find(type);
        if (it2 != _myClipboardSource.end()) {
            _myClipboardContents[type] = it2->second;
            return it2->second;
        }
    } else if (currentOffer) {
        auto it2 = std::find(_currentMimeTypes.begin(), _currentMimeTypes.end(), type);
        if (it2 != _currentMimeTypes.end()) {
            auto mimeType = *it2;
            // pull down offer
            int fds[2];
            pipe(fds);
            wl_data_offer_receive(currentOffer, mimeType.c_str(), fds[1]);
            wl_display_flush(display);
            close(fds[1]);
            ByteBuf res;

            while (true) {
                char buf[1024];
                ssize_t n = read(fds[0], buf, sizeof(buf));
                if (n <= 0)
                    break;
                res.insert(res.end(), buf, buf + n);
            }
            // cache
            _myClipboardContents[mimeType] = res;
            close(fds[0]);
            return res;
        }
    }
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

static void dataSourceSend(void* data, wl_data_source* source, const char* mimeType, int fd) {
    auto self = reinterpret_cast<WindowManagerWayland*>(data);
    auto it = self->_myClipboardSource.find(std::string(mimeType));
    if (it != self->_myClipboardSource.end()) {
        write(fd, it->second.data(), it->second.size());
    }
    close(fd);
}
static void dataSourceCancelled(void* data, wl_data_source* source) {
    auto self = reinterpret_cast<WindowManagerWayland*>(data);
    wl_data_source_destroy(source);
    self->currentSource = nullptr;
    self->_myClipboardSource = {};
}
wl_data_source_listener WindowManagerWayland::_sourceListener = {
    .send = dataSourceSend,
    .cancelled = dataSourceCancelled 
};
void WindowManagerWayland::setClipboardContents(std::map<std::string, ByteBuf>&& c) {
    assert(("create at least one window in order to use clipboard" && !_nativeWindowToMy.empty()));
    _myClipboardSource = c;

    if (!deviceManager) return;

    currentSource = wl_data_device_manager_create_data_source(deviceManager);
    
    wl_data_source_add_listener(currentSource, &_sourceListener, this);

    _currentMimeTypes = {};
    for (auto it : c) {
        _currentMimeTypes.push_back(it.first.c_str());
        wl_data_source_offer(currentSource, it.first.c_str());
    }
    
    if (getKeyboardSerial() > 0)
        wl_data_device_set_selection(dataDevice, currentSource, getKeyboardSerial());

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
