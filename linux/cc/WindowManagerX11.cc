#include "WindowManagerX11.hh"
#include "WindowX11.hh"
#include <cstdio>
#include <limits>
#include <impl/Library.hh>
#include <impl/JNILocal.hh>
#include "App.hh"

using namespace jwm;

int WindowManagerX11::_xerrorhandler(Display* dsp, XErrorEvent* error) {
    char errorstring[0x100];
    XGetErrorText(dsp, error->error_code, errorstring, sizeof(errorstring));
    printf("X Error: %s\n", errorstring);
    return 0;
}

WindowManagerX11::WindowManagerX11() {
    display = XOpenDisplay(nullptr);
    XSetErrorHandler(_xerrorhandler);
    screen = DefaultScreenOfDisplay(display);

    // for utf8 input
    if (XSupportsLocale()) {
        XSetLocaleModifiers("@im=none");

        im = XOpenIM(display, NULL, NULL, NULL);
        if (im != NULL) {
            XIMStyles* styles;
            if (XGetIMValues(im, XNQueryInputStyle, &styles, NULL)) {
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
                          | PropertyChangeMask;
    }
}

void WindowManagerX11::runLoop() {
    XEvent ev;
    for (;;) {
        using namespace classes;
        while (XPending(display)) {
            XNextEvent(display, &ev);
            WindowX11* myWindow = nullptr;
            auto it = _nativeWindowToMy.find(ev.xkey.window);
            if (it != _nativeWindowToMy.end()) {
                myWindow = it->second;
            }

            switch (ev.type) {
                case Expose: { // resize
                    jwm::JNILocal<jobject> eventResize(app.getJniEnv(), EventResize::make(app.getJniEnv(), ev.xexpose.width, ev.xexpose.height));
                    myWindow->dispatch(eventResize.get());
                    break;
                }

                case MotionNotify: { // mouse move
                    jwm::JNILocal<jobject> eventMove(app.getJniEnv(), EventMouseMove::make(app.getJniEnv(), ev.xmotion.x, ev.xmotion.y));
                    myWindow->dispatch(eventMove.get());
                    break;
                }

                case KeyPress: { // keyboard down
                    jwm::JNILocal<jobject> eventKeyboard(app.getJniEnv(), EventKeyboard::make(app.getJniEnv(), ev.xkey.keycode, true));
                    myWindow->dispatch(eventKeyboard.get());
                    break;
                }

                case KeyRelease: { // keyboard down
                    jwm::JNILocal<jobject> eventKeyboard(app.getJniEnv(), EventKeyboard::make(app.getJniEnv(), ev.xkey.keycode, false));
                    myWindow->dispatch(eventKeyboard.get());
                    break;
                }
            }
        }

        for (auto& p : _nativeWindowToMy) {
            if (p.second->isRedrawRequested()) {
                p.second->unsetRedrawRequest();
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
