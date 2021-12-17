#include "WindowManagerX11.hh"
#include "WindowX11.hh"
#include <cstdio>
#include <limits>
#include <impl/Library.hh>
#include <impl/JNILocal.hh>
#include "AppX11.hh"
#include <X11/extensions/sync.h>
#include <X11/extensions/XInput2.h>
#include <X11/Xcursor/Xcursor.h>
#include <X11/cursorfont.h>
#include <X11/Xatom.h>
#include "KeyX11.hh"
#include "MouseButtonX11.hh"
#include "StringUTF16.hh"
#include <algorithm>
#include "Log.hh"

using namespace jwm;

int WindowManagerX11::_xerrorhandler(Display* dsp, XErrorEvent* error) {
    char errorstring[0x100];
    XGetErrorText(dsp, error->error_code, errorstring, sizeof(errorstring));
    printf("X Error: %s\n", errorstring);
    return 0;
}

WindowManagerX11::WindowManagerX11():
    display(XOpenDisplay(nullptr)),
    _atoms(display) {

    XSetErrorHandler(_xerrorhandler);
    screen = DefaultScreenOfDisplay(display);

    // for utf8 input
    if (XSupportsLocale()) {
        XSetLocaleModifiers("@im=none");

        _im = XOpenIM(display, NULL, NULL, NULL);
        if (_im != NULL) {
            XIMStyles* styles;
            if (XGetIMValues(_im, XNQueryInputStyle, &styles, NULL)) {
                // could not init IM
                throw std::runtime_error("failed to init IM");
            }
        }
    }

    // pick visual info
    {
        GLint att[] = {
            GLX_X_RENDERABLE, True, // 1
            GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT, // 3
            GLX_RENDER_TYPE, GLX_RGBA_BIT, // 5
            GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR, // 7
            GLX_RED_SIZE, 8, // 9
            GLX_GREEN_SIZE, 8, // 11
            GLX_BLUE_SIZE, 8, // 13
            GLX_ALPHA_SIZE, 8, // 15
            GLX_DEPTH_SIZE, 24,
            GLX_STENCIL_SIZE, 8,
            GLX_DOUBLEBUFFER, true,
            GLX_STENCIL_SIZE, 8,
            None
        };

        int fbcount;
        GLXFBConfig* fbc = glXChooseFBConfig(display, DefaultScreen(display), att, &fbcount);

        if (fbc == nullptr || fbcount <= 0) {
            // try to reduce system requirements
            if (fbc == nullptr || fbcount <= 0) {
                // try to disable rgba.
                att[5] = 0;
                fbc = glXChooseFBConfig(display, DefaultScreen(display), att, &fbcount);

                if (fbc == nullptr || fbcount <= 0) {
                    // use default attribs
                    glXChooseFBConfig(display, DefaultScreen(display), nullptr, &fbcount);
                    if (fbc == nullptr || fbcount <= 0) {
                        // giving up.
                        x11VisualInfo = nullptr;
                        JWM_LOG("Failed to pick OpenGL-capable X11 VisualInfo; OpenGL is not available");
                    }
                }
            }
        }

        if (fbc) {
            // pick the FB config/visual with the most samples per pixel
            int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = std::numeric_limits<int>::max();

            XVisualInfo* vi;
            int i;
            for (i = 0; i < fbcount; ++i) {
                vi = glXGetVisualFromFBConfig(display, fbc[i]);
                if (vi) {
                    int samp_buf, samples;
                    glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
                    glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLES, &samples);

                    if (best_fbc < 0 || samp_buf && samples > best_num_samp)
                        best_fbc = i, best_num_samp = samples;
                    if (worst_fbc < 0 || !samp_buf || samples < worst_num_samp)
                        worst_fbc = i, worst_num_samp = samples;
                }
                XFree(vi);
            }

            GLXFBConfig bestFbc = fbc[best_fbc];
            XFree(fbc);

            // get a visual
            x11VisualInfo = glXGetVisualFromFBConfig(display, bestFbc);
        }
    }

    // create swa
    {
        x11SWA.colormap = XCreateColormap(display, screen->root, getX11Visual(), AllocNone);
        x11SWA.event_mask = ExposureMask
                        | KeyPressMask
                        | KeyReleaseMask
                        | ButtonPressMask
                        | ButtonReleaseMask
                        | StructureNotifyMask
                        | PointerMotionMask
                        | PropertyChangeMask
                        | StructureNotifyMask;
        x11SWA.override_redirect = true;
    }

    // XInput2
    {
        int opcode, firstevent, firsterror;
        if (XQueryExtension(display, "XInputExtension", &opcode, &firstevent, &firsterror)) {
            int major = 2, minor = 3;
            if (XIQueryVersion(display, &major, &minor) != BadRequest) {
                _xi2 = std::make_unique<XInput2>(XInput2{ opcode });
                _xi2IterateDevices();
            }
        }
    }

    // load system cursors
    {
        const char* themeName = XcursorGetTheme(display);
        int defaultSize = themeName != nullptr ? XcursorGetDefaultSize(display) : 0;
        auto loadCursor = [&](const char* name, unsigned alternativeId) {
            if (themeName) {
                auto image = XcursorLibraryLoadImage(name, themeName, defaultSize);
                if (image) {
                    auto cursor = XcursorImageLoadCursor(display, image);
                    XcursorImageDestroy(image);
                    return cursor;
                } 
            }

            // fallback to non-theme cursor
            return XCreateFontCursor(display, alternativeId);
        };
        
        _cursors[static_cast<int>(jwm::MouseCursor::ARROW         )] = loadCursor("default"     , XC_left_ptr );
        _cursors[static_cast<int>(jwm::MouseCursor::CROSSHAIR     )] = loadCursor("crosshair"   , XC_left_ptr );
        _cursors[static_cast<int>(jwm::MouseCursor::HELP          )] = loadCursor("help"        , XC_left_ptr );
        _cursors[static_cast<int>(jwm::MouseCursor::POINTING_HAND )] = loadCursor("pointer"     , XC_hand2    );
        _cursors[static_cast<int>(jwm::MouseCursor::IBEAM         )] = loadCursor("text"        , XC_xterm    );
        _cursors[static_cast<int>(jwm::MouseCursor::NOT_ALLOWED   )] = loadCursor("not-allowed" , XC_left_ptr );
        _cursors[static_cast<int>(jwm::MouseCursor::WAIT          )] = loadCursor("watch"       , XC_left_ptr );
    }
}

void WindowManagerX11::_xi2IterateDevices() {
    int deviceCount;
    XIDeviceInfo* devices = XIQueryDevice(display, XIAllDevices, &deviceCount);
    for (int deviceId = 0; deviceId < deviceCount; ++deviceId) {
        XIDeviceInfo& device = devices[deviceId];
        if (device.use != XIMasterPointer) {
            continue;
        }
        XInput2::Device& myDevice = _xi2->deviceById[device.deviceid];
        for (int classId = 0; classId < device.num_classes; ++classId) {
            XIAnyClassInfo* classInfo = device.classes[classId];

            switch (classInfo->type)
            {
                case XIScrollClass: {
                    XIScrollClassInfo* scroll = reinterpret_cast<XIScrollClassInfo*>(classInfo);
                    myDevice.scroll.push_back(XInput2::Device::ScrollValuator{
                        scroll->scroll_type == XIScrollTypeHorizontal,
                        scroll->number,
                        scroll->increment
                    });
                    break;
                }
            }
        }
    }
    XIFreeDeviceInfo(devices);
}

::Window WindowManagerX11::getRootWindow() const {
    return XDefaultRootWindow(display);
}


void WindowManagerX11::runLoop() {
    _runLoop = true;
    XEvent ev;
    while (_runLoop) {
        while (XPending(display)) {
            XNextEvent(display, &ev);
            _processXEvent(ev);
            if (jwm::classes::Throwable::exceptionThrown(app.getJniEnv()))
                _runLoop = false;
        }

        _processCallbacks();

    }
}

void WindowManagerX11::_processCallbacks() {
    {
        // process ui thread callbacks
        std::unique_lock<std::mutex> lock(_taskQueueLock);
        
        // TODO better solution for handling both XPending and condition_variable
        _taskQueueNotify.wait_for(lock, std::chrono::microseconds(500));

        while (!_taskQueue.empty()) {
            auto callback = std::move(_taskQueue.front());
            _taskQueue.pop();
            lock.unlock();
            callback();
            lock.lock();
        }        
    }
    {
        // process redraw requests
        for (auto& p : _nativeWindowToMy) {
            if (p.second->isRedrawRequested()) {
                p.second->unsetRedrawRequest();
                if (p.second->_layer) {
                    p.second->_layer->makeCurrent();
                }
                p.second->dispatch(classes::EventFrame::kInstance);
            }
        }
    }
}

void WindowManagerX11::_processXEvent(XEvent& ev) {
    using namespace classes;

    WindowX11* myWindow = nullptr;
    auto it = _nativeWindowToMy.find(ev.xkey.window);
    if (it != _nativeWindowToMy.end()) {
        myWindow = it->second;
    }
    if (myWindow == nullptr) {
        // probably an XI2 event
        if (ev.type == GenericEvent && _xi2 && _xi2->opcode == ev.xcookie.extension) {
            if (XGetEventData(display, &ev.xcookie)) {
                XIEvent* xiEvent = reinterpret_cast<XIEvent*>(ev.xcookie.data);
                switch (xiEvent->evtype) {
                    case XI_DeviceChanged:
                        _xi2->deviceById.clear();
                        _xi2IterateDevices();
                        break;

                    case XI_Motion: {
                        XIDeviceEvent* deviceEvent = reinterpret_cast<XIDeviceEvent*>(xiEvent);
                        
                        it = _nativeWindowToMy.find(deviceEvent->event);

                        if (it != _nativeWindowToMy.end()) {
                            myWindow = it->second;
                        } else {
                            break;
                        }

                        if (myWindow->_layer) {
                            myWindow->_layer->makeCurrent();
                        }

                        auto itMyDevice = _xi2->deviceById.find(deviceEvent->deviceid);
                        if (itMyDevice == _xi2->deviceById.end()) {
                            break;
                        }

                        XInput2::Device& myDevice = itMyDevice->second;
                        double dX = 0, dY = 0;
                        for (int i = 0, valuatorIndex = 0; i < deviceEvent->valuators.mask_len * 8; ++i) {
                            if (!XIMaskIsSet(deviceEvent->valuators.mask, i)) {
                                continue;
                            }

                            for (auto& valuator : myDevice.scroll) {
                                if (valuator.number == i) {
                                    double value = reinterpret_cast<double*>(deviceEvent->valuators.values)[valuatorIndex];
                                    if (valuator.previousValue == 0) {
                                        valuator.previousValue = value;
                                        value = 0;
                                    } else {
                                        double delta = value - valuator.previousValue;
                                        valuator.previousValue = value;
                                        value = delta;                                                
                                    }
                                    if (valuator.isHorizontal) {
                                        dX = value;
                                    } else {
                                        dY = value;
                                    }
                                    break;
                                }
                            }
                            ++valuatorIndex;
                        }

                        if (dX != 0 || dY != 0) {
                            jwm::JNILocal<jobject> eventMouseScroll(
                                app.getJniEnv(),
                                EventMouseScroll::make(
                                    app.getJniEnv(),
                                    -dX,
                                    -dY,
                                    0.0f,
                                    0.0f,
                                    0.0f,
                                    jwm::KeyX11::getModifiers()
                                )
                            );
                            myWindow->dispatch(eventMouseScroll.get()); 
                        }

                        break;
                    }
                }
                XFreeEventData(display, &ev.xcookie);
            }
        }
        return;
    }
    if (myWindow->_layer) {
        myWindow->_layer->makeCurrent();
    }

    switch (ev.type) {
        case ClientMessage: {
            if (ev.xclient.message_type == _atoms.WM_PROTOCOLS) {
                if (ev.xclient.data.l[0] == _atoms._NET_WM_SYNC_REQUEST) {
                    // flicker-fix sync on resize
                    myWindow->_xsyncRequestCounter.lo = ev.xclient.data.l[2];
                    myWindow->_xsyncRequestCounter.hi = ev.xclient.data.l[3];
                } else if (ev.xclient.data.l[0] == _atoms.WM_DELETE_WINDOW) {
                    // close button clicked
                    myWindow->dispatch(EventWindowCloseRequest::kInstance);
                }
            }
            break;
        }
        case ConfigureNotify: { // resize and move
            WindowX11* except = nullptr;
            if (ev.xconfigure.x != myWindow->_posX || ev.xconfigure.y != myWindow->_posY) {
                myWindow->_posX = ev.xconfigure.x;
                myWindow->_posY = ev.xconfigure.y;

                jwm::JNILocal<jobject> eventMove(
                    app.getJniEnv(),
                    EventWindowMove::make(
                        app.getJniEnv(),
                        ev.xconfigure.x,
                        ev.xconfigure.y
                    )
                );
                myWindow->dispatch(eventMove.get()); 
            }
            if (ev.xconfigure.width != myWindow->_width || ev.xconfigure.height != myWindow->_height)
            {
                except = myWindow;
                myWindow->_width = ev.xconfigure.width;
                myWindow->_height = ev.xconfigure.height;
                jwm::JNILocal<jobject> eventWindowResize(app.getJniEnv(), EventWindowResize::make(app.getJniEnv(), ev.xconfigure.width, ev.xconfigure.height, ev.xconfigure.width, ev.xconfigure.height));
                myWindow->dispatch(eventWindowResize.get());

                // force redraw
                if (myWindow->_layer) {
                    myWindow->_layer->makeCurrent();
                    myWindow->_layer->setVsyncMode(ILayer::VSYNC_DISABLED);
                }
                myWindow->dispatch(EventFrame::kInstance);

                if (myWindow->_layer) {
                    myWindow->_layer->setVsyncMode(ILayer::VSYNC_ADAPTIVE);
                }

                XSyncValue syncValue;
                XSyncIntsToValue(&syncValue,
                                myWindow->_xsyncRequestCounter.lo,
                                myWindow->_xsyncRequestCounter.hi);
                XSyncSetCounter(display, myWindow->_xsyncRequestCounter.counter, syncValue);


                // force repaint all windows otherwise they will freeze on GTK-based WMs
                for (auto& p : _nativeWindowToMy) {
                    if (except != p.second && p.second->isRedrawRequested()) {
                        p.second->unsetRedrawRequest();
                        if (p.second->_layer) {
                            p.second->_layer->makeCurrent();
                        }
                        p.second->dispatch(EventFrame::kInstance);
                    }
                }
            }

            break;
        }

        case MotionNotify: { // mouse move
            unsigned mask;
            ::Window unused1;
            int unused2;
            XQueryPointer(display, myWindow->_x11Window, &unused1, &unused1, &unused2, &unused2, &unused2, &unused2, &mask);
            jwm::JNILocal<jobject> eventMove(
                app.getJniEnv(),
                EventMouseMove::make(app.getJniEnv(),
                    ev.xmotion.x,
                    ev.xmotion.y,
                    jwm::MouseButtonX11::fromNativeMask(mask),
                    jwm::KeyX11::getModifiers()
                )
            );
            myWindow->dispatch(eventMove.get());
            break;
        }

        case ButtonPress: { // mouse down
            jwm::JNILocal<jobject> eventButton(
                app.getJniEnv(),
                EventMouseButton::make(
                    app.getJniEnv(),
                    MouseButtonX11::fromNative(ev.xbutton.button),
                    true,
                    jwm::KeyX11::getModifiers()
                )
            );
            myWindow->dispatch(eventButton.get());
            break;
        }

        case ButtonRelease: { // mouse down
            jwm::JNILocal<jobject> eventButton(
                app.getJniEnv(),
                EventMouseButton::make(
                    app.getJniEnv(),
                    MouseButtonX11::fromNative(ev.xbutton.button),
                    false,
                    jwm::KeyX11::getModifiers()
                )
            );
            myWindow->dispatch(eventButton.get());
            break;
        }

        case KeyPress: { // keyboard down
            KeySym s = XLookupKeysym(&ev.xkey, 0);
            jwm::Key key = KeyX11::fromNative(s);
            jwm::KeyX11::setKeyState(key, true);
            jwm::JNILocal<jobject> eventKey(app.getJniEnv(),
                                                    EventKey::make(app.getJniEnv(),
                                                                        key,
                                                                        true,
                                                                        jwm::KeyX11::getModifiers()));
            myWindow->dispatch(eventKey.get());
            uint8_t textBuffer[0x20];
            Status status;
            int count = Xutf8LookupString(myWindow->_ic,
                                          (XKeyPressedEvent*)&ev,
                                          reinterpret_cast<char*>(textBuffer),
                                          sizeof(textBuffer),
                                          &s,
                                          &status);
            textBuffer[count] = 0;
            if (count > 0) {
                // ignore delete key and other control symbols
                if (textBuffer[0] != 127 && textBuffer[0] > 0x1f) {
                    JNIEnv* env = app.getJniEnv();

                    jwm::StringUTF16 converted = reinterpret_cast<const char*>(textBuffer);
                    jwm::JNILocal<jstring> jtext = converted.toJString(env);

                    jwm::JNILocal<jobject> eventTextInput(env, EventTextInput::make(env, jtext.get()));
                    myWindow->dispatch(eventTextInput.get());
                }
            }

            break;
        }

        case KeyRelease: { // keyboard down
            KeySym s = XLookupKeysym(&ev.xkey, 0);
            jwm::Key key = KeyX11::fromNative(s);
            jwm::KeyX11::setKeyState(key, false);
            jwm::JNILocal<jobject> eventKey(app.getJniEnv(),
                                                    EventKey::make(app.getJniEnv(),
                                                                        key,
                                                                        false,
                                                                        jwm::KeyX11::getModifiers()));
            myWindow->dispatch(eventKey.get());
            break;
        }

        case SelectionRequest: {
            if (ev.xselectionrequest.property == None) {
                break;
            }
            if (ev.xselectionrequest.target == _atoms.TARGETS) { // data type request
                std::vector<Atom> targetAtoms = {
                        XInternAtom(display, "TIMESTAMP", false),
                        XInternAtom(display, "TARGETS", false),
                        XInternAtom(display, "SAVE_TARGETS", false),
                        XInternAtom(display, "MULTIPLE", false)
                };
                // additional 10 elements to be sure
                targetAtoms.reserve(10 + _myClipboardContents.size());
                for (auto& entry : _myClipboardContents) {
                    targetAtoms.push_back(XInternAtom(display, entry.first.c_str(), false));
                }

                // if user stored text, we should also add UTF8_STRING and STRING
                if (_myClipboardContents.find("text/plain") != _myClipboardContents.end()) {
                    targetAtoms.insert(targetAtoms.end(),
                                 {
                                     XInternAtom(display, "STRING", false),
                                     XInternAtom(display, "UTF8_STRING", false),
                                     XInternAtom(display, "text/plain;charset=utf-8", false),
                                 });
                }

                XChangeProperty(display,
                                ev.xselectionrequest.requestor,
                                ev.xselectionrequest.property,
                                ev.xselectionrequest.target,
                                targetAtoms.size(),
                                PropModeReplace,
                                (unsigned char*) targetAtoms.data(),
                                sizeof(Atom) * targetAtoms.size());
            } else { // data request
                std::string targetName;
                {
                    char* targetNameC = XGetAtomName(display, ev.xselectionrequest.target);
                    targetName = targetNameC;
                    XFree(targetNameC);
                }
                auto it = _myClipboardContents.find(targetName);
                if (it == _myClipboardContents.end() && (targetName == "UTF8_STRING" || targetName == "STRING" || targetName == "text/plain;charset=utf-8")) { // check for UTF8_STRING
                    it = _myClipboardContents.find("text/plain");
                }
                if (it != _myClipboardContents.end()) {
                    XChangeProperty(display,
                                    ev.xselectionrequest.requestor,
                                    ev.xselectionrequest.property,
                                    ev.xselectionrequest.target,
                                    8,
                                    PropModeReplace,
                                    (unsigned char*) it->second.data(),
                                    it->second.size());
                } else {
                    // we cannot supply contents
                    ev.xselectionrequest.property = None;
                }
            }

            // notify the requestor
            XSelectionEvent ssev;
            ssev.type = SelectionNotify;
            ssev.requestor = ev.xselectionrequest.requestor;
            ssev.selection = ev.xselectionrequest.selection;
            ssev.target = ev.xselectionrequest.target;
            ssev.property = ev.xselectionrequest.property;
            ssev.time = ev.xselectionrequest.time;

            XSendEvent(display, ev.xselectionrequest.requestor, True, NoEventMask, (XEvent *)&ssev);
            break;
        }
    }
}


std::vector<std::string> WindowManagerX11::getClipboardFormats() {
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

jwm::ByteBuf WindowManagerX11::getClipboardContents(const std::string& type) {
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

void WindowManagerX11::registerWindow(WindowX11* window) {
    _nativeWindowToMy[window->_x11Window] = window;

    // XInput
    if (_xi2) {
        XIEventMask eventMask;
        unsigned char mask[2] = { 0 };
        XISetMask(mask, XI_DeviceChanged);
        XISetMask(mask, XI_Motion);
        eventMask.deviceid = XIAllDevices;
        eventMask.mask_len = sizeof(mask);
        eventMask.mask = mask;

        XISelectEvents(display, window->_x11Window, &eventMask, 1);
    }
}

void WindowManagerX11::unregisterWindow(WindowX11* window) {
    auto it = _nativeWindowToMy.find(window->_x11Window);
    if (it != _nativeWindowToMy.end()) {
        _nativeWindowToMy.erase(it);
    }
}

void WindowManagerX11::terminate() {
    _runLoop = false;
}

void WindowManagerX11::setClipboardContents(std::map<std::string, ByteBuf>&& c) {
    assert(("create at least one window in order to use clipboard" && !_nativeWindowToMy.empty()));
    _myClipboardContents = c;
    XSetSelectionOwner(display, _atoms.CLIPBOARD, _nativeWindowToMy.begin()->first, CurrentTime);
}

void WindowManagerX11::enqueueTask(const std::function<void()>& task) {
    std::unique_lock<std::mutex> lock(_taskQueueLock);
    _taskQueue.push(task);
    _taskQueueNotify.notify_one();
}