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
                return wl_cursor_image_get_buffer(cursorImage);
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
        }

}




void WindowManagerWayland::runLoop() {
    _runLoop = true;
    XEvent ev;

    // buffer to read into; really only needs to be two characters long due to the notifyBool fast path, but longer doesn't hurt
    char buf[100];
    // initialize a pipe to write to whenever this loop needs to process something new
    int pipes[2];
    if (pipe(pipes)) {
        printf("Failed to open pipe\n");
        return;
    }

    notifyFD = pipes[1];
    fcntl(pipes[1], F_SETFL, O_NONBLOCK); // make sure notifyLoop doesn't block
    // two polled items - the X11 event queue, and our event queue
    struct pollfd ps[] = {{.fd=XConnectionNumber(display), .events=POLLIN}, {.fd=pipes[0], .events=POLLIN}};

    while (_runLoop) {
        while (XPending(display)) {
            XNextEvent(display, &ev);
            _processXEvent(ev);
            if (jwm::classes::Throwable::exceptionThrown(app.getJniEnv()))
                _runLoop = false;
        }
        _processCallbacks();

        // block until the next X11 or our event
        if (poll(&ps[0], 2, -1) < 0) {
            printf("Error during poll\n");
            break;
        }

        // clear pipe
        if (ps[1].revents & POLLIN) {
            while (read(pipes[0], buf, sizeof(buf)) == sizeof(buf)) { }
        }
        // clear fast path boolean; done after clearing the pipe so that, during event execution, new notifyLoop calls can still function
        notifyBool.store(false);
        // The events causing a notifyLoop anywhere between poll() end and here will be processed in all cases, as that's the next thing that happens
    }
    
    notifyFD = -1;
    close(pipes[0]);
    close(pipes[1]);
}

void WindowManagerWayland::notifyLoop() {
    if (notifyFD==-1) return;
    // fast notifyBool path to not make system calls when not necessary
    if (!notifyBool.exchange(true)) {
        char dummy[1] = {0};
        int unused = write(notifyFD, dummy, 1); // this really shouldn't fail, but if it does, the pipe should either be full (good), or dead (bad, but not our business)
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
        std::vector<WindowX11*> copy;
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
        compositor = wl_registry_bind(registry, name,
                &wl_compositor_interface, 3);
    } else if (strcmp(interface, "wl_shm") == 0) {
        shm = wl_registry_bind(registry, name,
                &wl_shm_interface, 1);
    } else if (strcmp(interface, "zxdg_shell_v6") == 0) {
        xdgShell = wl_registry_bind(registry, name,
                &zxdg_shell_v6_interface, 1);
    } else if (strcmp(interface, "wl_data_device_manager") == 0) {
        deviceManager = wl_registry_bind(registry, name,
                &wl_data_device_manager_interface, 1);
    } else if (strcmp(interface, "wl_seat") == 0) {
        seat = wl_registry_bind(registry, name,
                &wl_seat_interface, 1);
    }
}
void WindowManagerWayland::registryHandleGlobalRemove(void* data, wl_registry *registry, uint32_t name) {
    // i do nothing : )
}
std::vector<std::string> WindowManagerWayland::getClipboardFormats() {
    auto owner = XGetSelectionOwner(display, _atoms.CLIPBOARD);
    if (owner == None)
    {
        return {};
    }
    
    assert(("create at least one window in order to use clipboard" && !_nativeWindowToMy.empty()));

    auto nativeHandle = _nativeWindowToMy.begin()->first;
    assert(nativeHandle);

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
    return result;
}

jwm::ByteBuf WindowManagerWayland::getClipboardContents(const std::string& type) {
    auto nativeHandle = _nativeWindowToMy.begin()->first;

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
    return {};
}

void WindowManagerWayland::registerWindow(WindowWayland* window) {
    _nativeWindowToMy[window->_waylandWindow] = window;
}

void WindowManagerWayland::unregisterWindow(WindowX11* window) {
    auto it = _nativeWindowToMy.find(window->_waylandWindow);
    if (it != _nativeWindowToMy.end()) {
        _nativeWindowToMy.erase(it);
    }
}

void WindowManagerWayland::terminate() {
    _runLoop = false;
    notifyLoop();
}

void WindowManagerWayland::setClipboardContents(std::map<std::string, ByteBuf>&& c) {
    assert(("create at least one window in order to use clipboard" && !_nativeWindowToMy.empty()));
    _myClipboardContents = c;
    ::Window window = _nativeWindowToMy.begin()->first;
    XSetSelectionOwner(display, XA_PRIMARY, window, CurrentTime);
    XSetSelectionOwner(display, _atoms.CLIPBOARD, window, CurrentTime);
}

void WindowManagerWayland::enqueueTask(const std::function<void()>& task) {
    std::unique_lock<std::mutex> lock(_taskQueueLock);
    _taskQueue.push(task);
    _taskQueueNotify.notify_one();
    notifyLoop();
}
