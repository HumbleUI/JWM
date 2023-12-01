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
#include "xdg-shell.hh"
#include <cstring>

using namespace jwm;


WindowManagerWayland::WindowManagerWayland():
    display(wl_display_connect(nullptr)) {
        registry = wl_display_get_registry(display);
        wl_registry_listener registry_listener = {
            .global = registryGlobalHandler,
            .global_remove = registryGlobalHandlerRemove
        };
        wl_registry_add_listener(registry, &registry_listener, nullptr);

        wl_display_roundtrip(display);

        if (!(shm && xdgShell && compositor && deviceManager && seat)) {
            // ???
            // Bad. Means our compositor no supportie : (
            throw std::system_error(1, std::system_category);
        }

        

        {
            wl_cursor_theme* cursor_theme = wl_cursor_theme_load(nullptr, 24, shm);
            // TODO: what about if missing : (
            auto loadCursor = [&](const char* name) {
                wl_cursor* cursor = wl_cursor_theme_get_cursor(cursor_theme, name);
                wl_cursor_image* cursorImage = cursor->images[0];
                return cursorImage;
            }

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
                    wl_cursor_get_image_buffer(_cursors[static_cast<int>(jwm::MouseCursor::ARROW)]), 0, 0);
            wl_surface_commit(cursorSurface);
        }

        {
            pointer = wl_seat_get_pointer(seat);
            wl_pointer_listener pointerListener = {
              .enter = pointerHandleEnter,
              .leave = pointerHandleLeave,
              .motion = pointerHandleMotion,
              .button = pointerHandleButton,
              .axis = pointerHandleAxis
            };
            wl_pointer_add_listener(pointer, &pointerListener, nullptr);
        }



}




void WindowManagerWayland::runLoop() {
    _runLoop = true;

    // who be out here running they loop
    while (_runLoop) {
    }
    
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
        std::vector<WindowWindow*> copy;
        for (auto& p : _nativeWindowToMy) {
            copy.push_back(p.second);
        }
        // process redraw requests
        for (auto p : copy) {
            if (p->isRedrawRequested()) {
                p->unsetRedrawRequest();
                if (p->_layer) {
                    p->_layer->makeCurrent();
                }
                p->dispatch(classes::EventFrame::kInstance);
            }
        }
    }
}

void WindowManagerWayland::registryHandleGlobal(void* data, wl_registry *registry,
        uint32_t name, const char* interface, uint32_t version) {
    if (strcmp(interface, "wl_compositor") == 0) {
        compositor = (wl_compositor*)wl_registry_bind(registry, name,
                &wl_compositor_interface, 3);
    } else if (strcmp(interface, "wl_shm") == 0) {
        shm = (wl_shm*)wl_registry_bind(registry, name,
                &wl_shm_interface, 1);
    } else if (strcmp(interface, "xdg_wm_base") == 0) {
        xdgShell = (xdg_wm_base*)wl_registry_bind(registry, name,
                &xdg_wm_base_interface, 1);
    } else if (strcmp(interface, "wl_data_device_manager") == 0) {
        deviceManager = (wl_data_device_manager*)wl_registry_bind(registry, name,
                &wl_data_device_manager_interface, 1);
    } else if (strcmp(interface, "wl_seat") == 0) {
        seat = (wl_seat*)wl_registry_bind(registry, name,
                &wl_seat_interface, 1);
    }
}
void WindowManagerWayland::registryHandleGlobalRemove(void* data, wl_registry *registry, uint32_t name) {
    // i do nothing : )
}

void WindowManagerWayland::pointerHandleEnter(void* data, wl_pointer* pointer, uint32_t serial,
        wl_surface *surface, wl_fixed_t surface_x, wl_fixed_t surface_y) {
    wl_cursor_image* image = _cursors[static_cast<int>(jwm::MouseCursor::ARROW)];
    wl_pointer_set_cursor(cursor, serial, cursorSurface,  image->hotspot_x, image->hotspot_y);
    focusedSurface = surface;
}
void WindowManagerWayland::pointerHandleLeave(void* data, wl_pointer* pointer, uint32_t serial,
        wl_surface *surface) {
    focusedSurface = nullptr;
    // ???
    mouseMask = 0;
}
void WindowManagerWayland::pointerHandleMotion(void* data, wl_pointer* pointer,
        uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y) {
    lastMousePosX = surface_x;
    lastMousePosY = surface_y;
    if (focusedSurface) {
        ::WindowWayland* window = _nativeWindowToMy.find(focusedSurface)->second;
        mouseUpdate(window, wl_fixed_to_int(surface_x), wl_fixed_to_int(surface_y), mouseMask);
    }
    
}
void WindowManagerWayland::pointerHandleButton(void* data, wl_pointer* pointer, 
        uint32_t serial, uint32_t time, uint32_t button, uint32_t state) {
    using namespace classes;
    if (state == 0) {
        // release
        switch (button) {
            // primary
            case 0x110:
                mouseMask &= ~0x100;
                break;
            // secondary
            case 0x111:
                mouseMask &= ~0x400;
                break;
            // middle
            case 0x112:
                mouseMask &= ~0x200;
                break;
            default:
                break;
        }
        
        if (MouseButtonWayland::isButton(button) && focusedSurface) {
            jwm::JNILocal<jobject> eventButton(
                    app.getJniEnv(),
                    EventMouseButton::make(
                            app.getJniEnv(),
                            MouseButtonWayland::fromNative(button),
                            false,
                            lastMousePosX,
                            lastMousePosY,
                            jwm::KeyWayland::getModifiers()
                        )
                    );
            WindowWayland* window = _nativeWindowToMy.find(focusedSurface)->second;
            window->dispatch(eventButton.get());
        }
    } else {
        // down
        switch (button) {
            // primary
            case 0x110:
                mouseMask |= 0x100;
                break;
            // secondary
            case 0x111:
                mouseMask |= 0x400;
                break;
            // middle
            case 0x112:
                mouseMask |= 0x200;
                break;
            default:
                break;
        }
        
        if (MouseButtonWayland::isButton(button) && focusedSurface) {
            jwm::JNILocal<jobject> eventButton(
                    app.getJniEnv(),
                    EventMouseButton::make(
                            app.getJniEnv(),
                            MouseButtonWayland::fromNative(button),
                            true,
                            lastMousePosX,
                            lastMousePosY,
                            jwm::KeyWayland::getModifiers()
                        )
                    );
            WindowWayland* window = _nativeWindowToMy.find(focusedSurface)->second;
            window->dispatch(eventButton.get());
        }
    }
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

    // impl me : )
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
            jwm::KeyWayland::getModifiersFromMask(0)
            )
        );
    myWindow->dispatch(eventMove.get());
}
jwm::ByteBuf WindowManagerWayland::getClipboardContents(const std::string& type) {
    auto nativeHandle = _nativeWindowToMy.begin()->first;
    /*
    XConvertSelection(display,
                      _atoms.CLIPBOARD,
                      XInternAtom(display, type.c_str(), false),
                      _atoms.JWM_CLIPBOARD,
                      nativeHandle,
                      CurrentTime);
    XEvent ev;
    while (_runLoop) {
        while (XPending(display)) {
            XNextEvent(display, &ev);
            switch (ev.type)
            {
                case SelectionNotify: {
                    if (ev.xselection.property == None) {
                        return {};
                    }
                    
                    Atom da, incr, type;
                    int di;
                    unsigned long size, length, count;
                    unsigned char* propRet = NULL;

                    XGetWindowProperty(display, nativeHandle, _atoms.JWM_CLIPBOARD, 0, 0, False, AnyPropertyType,
                                    &type, &di, &length, &size, &propRet);
                    XFree(propRet);

                    // Clipboard data is too large and INCR mechanism not implemented
                    ByteBuf result;
                    if (type != _atoms.INCR)
                    {
                        XGetWindowProperty(display, nativeHandle, _atoms.JWM_CLIPBOARD, 0, size, False, AnyPropertyType,
                                        &da, &di, &length, &count, &propRet);
                        
                        result = ByteBuf{ propRet, propRet + length };
                        XFree(propRet);
                        return result;
                    }
                    XDeleteProperty(display, nativeHandle, _atoms.JWM_CLIPBOARD);
                    return result;
                }
                default:
                    _processXEvent(ev);
            }
        }
        _processCallbacks();
    }

    XDeleteProperty(display, nativeHandle, _atoms.JWM_CLIPBOARD);
    */
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
