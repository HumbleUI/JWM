#include "WindowManagerX11.hh"
#include "WindowX11.hh"
#include <cstdio>
#include <limits>
#include <impl/Library.hh>
#include <impl/JNILocal.hh>
#include "AppX11.hh"
#include <X11/extensions/sync.h>
#include "KeyX11.hh"
#include "MouseButtonX11.hh"

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
                        return;
                    }
                }
            }
        }

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

    // create swa
    {
        x11SWA.colormap = XCreateColormap(display, screen->root, x11VisualInfo->visual, AllocNone);
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

}

::Window WindowManagerX11::getRootWindow() const {
    return XDefaultRootWindow(display);
}

void WindowManagerX11::runLoop() {
    _runLoop = true;
    XEvent ev;
    while (_runLoop) {
        using namespace classes;
        while (XPending(display)) {
            XNextEvent(display, &ev);
            WindowX11* myWindow = nullptr;
            auto it = _nativeWindowToMy.find(ev.xkey.window);
            if (it != _nativeWindowToMy.end()) {
                myWindow = it->second;
            }
            if (myWindow == nullptr) {
                continue;
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
                            myWindow->dispatch(EventClose::kInstance);
                        }
                    }
                    break;
                }
                case ConfigureNotify: { // resize and move
                    WindowX11* except = nullptr;
                    if (ev.xconfigure.width != myWindow->_width || ev.xconfigure.height != myWindow->_height)
                    {
                        except = myWindow;
                        myWindow->_width = ev.xconfigure.width;
                        myWindow->_height = ev.xconfigure.height;
                        jwm::JNILocal<jobject> eventResize(app.getJniEnv(), EventResize::make(app.getJniEnv(), ev.xconfigure.width, ev.xconfigure.height));
                        myWindow->dispatch(eventResize.get());

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
                    }

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
                    jwm::JNILocal<jobject> eventKeyboard(app.getJniEnv(),
                                                         EventKeyboard::make(app.getJniEnv(),
                                                                             key,
                                                                             true,
                                                                             jwm::KeyX11::getModifiers()));
                    myWindow->dispatch(eventKeyboard.get());
                    char textBuffer[0x20];
                    Status status;
                    int count = Xutf8LookupString(myWindow->_ic,
                                                  (XKeyPressedEvent*)&ev,
                                                  textBuffer,
                                                  sizeof(textBuffer),
                                                  &s,
                                                  &status);
                    textBuffer[count] = 0;
                    if (count > 0) {
                        // ignore delete key
                        if (textBuffer[0] != 127) {
                            jwm::JNILocal<jobject> eventTextInput(app.getJniEnv(),
                                                                  EventTextInput::make(app.getJniEnv(),
                                                                                       textBuffer));
                            myWindow->dispatch(eventTextInput.get());
                        }
                    }

                    break;
                }

                case KeyRelease: { // keyboard down
                    KeySym s = XLookupKeysym(&ev.xkey, 0);
                    jwm::Key key = KeyX11::fromNative(s);
                    jwm::KeyX11::setKeyState(key, false);
                    jwm::JNILocal<jobject> eventKeyboard(app.getJniEnv(),
                                                         EventKeyboard::make(app.getJniEnv(),
                                                                             key,
                                                                             false,
                                                                             jwm::KeyX11::getModifiers()));
                    myWindow->dispatch(eventKeyboard.get());
                    break;
                }
            }
        }

        for (auto& p : _nativeWindowToMy) {
            if (p.second->isRedrawRequested()) {
                p.second->unsetRedrawRequest();
                if (p.second->_layer) {
                    p.second->_layer->makeCurrent();
                }
                p.second->dispatch(EventFrame::kInstance);
            }
        }
    }
}


void WindowManagerX11::registerWindow(WindowX11* window) {
    _nativeWindowToMy[window->_x11Window] = window;
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