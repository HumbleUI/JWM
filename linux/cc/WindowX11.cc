#include "WindowX11.hh"
#include <jni.h>
#include <memory>
#include "AppX11.hh"
#include "impl/Library.hh"
#include "impl/JNILocal.hh"
#include <X11/Xatom.h>
#include <X11/extensions/sync.h>
#include "KeyX11.hh"
#include <limits>
#include "MouseButtonX11.hh"

using namespace jwm;

static int _xerrorhandler(Display* dsp, XErrorEvent* error) {
    char errorstring[0x100];
    XGetErrorText(dsp, error->error_code, errorstring, sizeof(errorstring));
    printf("X Error: %s\n", errorstring);
    return 0;
}



WindowX11::WindowX11(JNIEnv* env):
    jwm::Window(env),
    _atoms(jwm::app.getDisplay())
{
    XSetErrorHandler(_xerrorhandler);
    Display* display = jwm::app.getDisplay();
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

WindowX11::~WindowX11() {
    XSyncDestroyCounter(jwm::app.getDisplay(), _xsyncRequestCounter.counter);
    XSyncDestroyCounter(jwm::app.getDisplay(), _xsyncRequestCounter.extended_counter);
    close();
}

void WindowX11::close() {
    if (_x11Window) {
        XDestroyWindow(jwm::app.getDisplay(), _x11Window);
        _x11Window = 0;
    }
}

void WindowX11::getPosition(int& posX, int& posY) {
    int x, y;
    ::Window child;
    XWindowAttributes xwa;
    XTranslateCoordinates(jwm::app.getDisplay(),
                          _x11Window,
                          XRootWindow(jwm::app.getDisplay(), 0),
                          0, 0,
                          &x, &y,
                          &child);
    XGetWindowAttributes(jwm::app.getDisplay(), _x11Window, &xwa);
    
    posX = x - xwa.x;
    posY = y - xwa.y;
}

int WindowX11::getLeft() {
    int x, y;
    getPosition(x, y);
    return x;
}

int WindowX11::getTop() {
    int x, y;
    getPosition(x, y);
    return y;
}

int WindowX11::getWidth() {
    return _width;
}

int WindowX11::getHeight() {
    return _height;
}

float WindowX11::getScale() {
    return jwm::app.getScale();
}

bool WindowX11::init()
{
    _x11Window = XCreateWindow(jwm::app.getDisplay(),
                               screen->root,
                               0, 0,
                               800, 500,
                               0,
                               x11VisualInfo->depth,
                               InputOutput,
                               x11VisualInfo->visual,
                               CWColormap | CWEventMask | CWCursor,
                               &x11SWA
    );
    
    XSetWMProtocols(jwm::app.getDisplay(),
                    _x11Window,
                    &_atoms.WM_DELETE_WINDOW,
                    Atoms::PROTOCOL_COUNT);

    // IC
    {
        _ic = XCreateIC(_im,
                        XNInputStyle,
                        XIMPreeditNothing | XIMStatusNothing,
                        XNClientWindow,
                        _x11Window,
                        nullptr);

        XSetICFocus(_ic);
    }



    // XSync
    {
        XSyncValue value;
        XSyncIntToValue(&value, 0);
        _xsyncRequestCounter.counter = XSyncCreateCounter(jwm::app.getDisplay(), value);
        _xsyncRequestCounter.extended_counter = XSyncCreateCounter(jwm::app.getDisplay(), value);
        XChangeProperty(jwm::app.getDisplay(),
                        _x11Window,
                        _atoms._NET_WM_SYNC_REQUEST_COUNTER,
                        XA_CARDINAL,
                        32,
                        PropModeReplace,
                        (const unsigned char*)&_xsyncRequestCounter.counter, 2);
        
    }
    return true;
}

void WindowX11::runLoop() {
    _runLoop = true;
    XEvent ev;
    while (_runLoop) {
        using namespace classes;
        while (XPending(jwm::app.getDisplay())) {
            XNextEvent(jwm::app.getDisplay(), &ev);
            if (_layer) {
                _layer->makeCurrent();
            }
            switch (ev.type) {
                case ClientMessage: {
                    if (ev.xclient.message_type == _atoms.WM_PROTOCOLS) {
                        if (ev.xclient.data.l[0] == _atoms._NET_WM_SYNC_REQUEST) {
                            // flicker-fix sync on resize
                            _xsyncRequestCounter.lo = ev.xclient.data.l[2];
                            _xsyncRequestCounter.hi = ev.xclient.data.l[3];
                            printf("%u+%u\n", _xsyncRequestCounter.lo, _xsyncRequestCounter.hi);
                        } else if (ev.xclient.data.l[0] == _atoms.WM_DELETE_WINDOW) {
                            // close button clicked
                            dispatch(EventClose::kInstance);
                        }
                    }
                    break;
                }
                case ConfigureNotify: { // resize and move
                    WindowX11* except = nullptr;
                    if (ev.xconfigure.width != _width || ev.xconfigure.height != _height)
                    {

                        XSyncValue syncValue;
                        XSyncIntsToValue(&syncValue,
                                        _xsyncRequestCounter.lo,
                                        _xsyncRequestCounter.hi);
                        XSyncSetCounter(jwm::app.getDisplay(), _xsyncRequestCounter.extended_counter, syncValue);
                        
                        _width = ev.xconfigure.width;
                        _height = ev.xconfigure.height;
                        jwm::JNILocal<jobject> eventResize(fEnv, EventResize::make(fEnv, ev.xconfigure.width, ev.xconfigure.height));
                       
                        // force redraw
                        if (_layer) {
                            _layer->makeCurrent();
                            _layer->setVsyncMode(ILayer::VSYNC_DISABLED);
                        }
                        
                        dispatch(eventResize.get());

                        if (_layer) {
                            _layer->setVsyncMode(ILayer::VSYNC_ENABLED);
                        }

                        XSyncSetCounter(jwm::app.getDisplay(), _xsyncRequestCounter.counter, syncValue);
                    }

                    // force repaint all windows otherwise they will freeze on GTK-based WMs
                    /*
                    for (auto& p : _nativeWindowToMy) {
                        if (except != p.second && p.second->isRedrawRequested()) {
                            p.second->unsetRedrawRequest();
                            if (p.second->_layer) {
                                p.second->_layer->makeCurrent();
                            }
                            p.second->dispatch(EventFrame::kInstance);
                        }
                    }*/
                    break;
                }

                case MotionNotify: { // mouse move
                    unsigned mask;
                    ::Window unused1;
                    int unused2;
                    XQueryPointer(jwm::app.getDisplay(), _x11Window, &unused1, &unused1, &unused2, &unused2, &unused2, &unused2, &mask);
                    jwm::JNILocal<jobject> eventMove(
                        fEnv,
                        EventMouseMove::make(fEnv,
                            ev.xmotion.x,
                            ev.xmotion.y,
                            jwm::MouseButtonX11::fromNativeMask(mask),
                            jwm::KeyX11::getModifiers()
                        )
                    );
                    dispatch(eventMove.get());
                    break;
                }

                case ButtonPress: { // mouse down
                    jwm::JNILocal<jobject> eventButton(
                        fEnv,
                        EventMouseButton::make(
                            fEnv,
                            MouseButtonX11::fromNative(ev.xbutton.button),
                            true,
                            jwm::KeyX11::getModifiers()
                        )
                    );
                    dispatch(eventButton.get());
                    break;
                }

                case ButtonRelease: { // mouse down
                    jwm::JNILocal<jobject> eventButton(
                        fEnv,
                        EventMouseButton::make(
                            fEnv,
                            MouseButtonX11::fromNative(ev.xbutton.button),
                            false,
                            jwm::KeyX11::getModifiers()
                        )
                    );
                    dispatch(eventButton.get());
                    break;
                }

                case KeyPress: { // keyboard down
                    KeySym s = XLookupKeysym(&ev.xkey, 0);
                    jwm::Key key = KeyX11::fromNative(s);
                    jwm::KeyX11::setKeyState(key, true);
                    jwm::JNILocal<jobject> eventKeyboard(fEnv,
                                                         EventKeyboard::make(fEnv,
                                                                             key,
                                                                             true,
                                                                             jwm::KeyX11::getModifiers()));
                    dispatch(eventKeyboard.get());
                    char textBuffer[0x20];
                    Status status;
                    int count = Xutf8LookupString(_ic,
                                                  (XKeyPressedEvent*)&ev,
                                                  textBuffer,
                                                  sizeof(textBuffer),
                                                  &s,
                                                  &status);
                    textBuffer[count] = 0;
                    if (count > 0) {
                        // ignore delete key
                        if (textBuffer[0] != 127) {
                            jwm::JNILocal<jobject> eventTextInput(fEnv,
                                                                  EventTextInput::make(fEnv,
                                                                                       textBuffer));
                            dispatch(eventTextInput.get());
                        }
                    }

                    break;
                }

                case KeyRelease: { // keyboard down
                    KeySym s = XLookupKeysym(&ev.xkey, 0);
                    jwm::Key key = KeyX11::fromNative(s);
                    jwm::KeyX11::setKeyState(key, false);
                    jwm::JNILocal<jobject> eventKeyboard(fEnv,
                                                         EventKeyboard::make(fEnv,
                                                                             key,
                                                                             false,
                                                                             jwm::KeyX11::getModifiers()));
                    dispatch(eventKeyboard.get());
                    break;
                }
            }
        }

        if (isRedrawRequested()) {
            unsetRedrawRequest();
            if (_layer) {
                _layer->makeCurrent();
            }
            dispatch(EventFrame::kInstance);
        }
    }
    XCloseDisplay(jwm::app.getDisplay());
}

void WindowX11::move(int left, int top) {
    XMoveWindow(jwm::app.getDisplay(), _x11Window, left, top);
}

void WindowX11::resize(int width, int height) {
    _width = width;
    _height = height;
    XResizeWindow(jwm::app.getDisplay(), _x11Window, width, height);
}


void WindowX11::show() {
    XMapWindow(jwm::app.getDisplay(), _x11Window);
}

// JNI

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_WindowX11__1nMake
  (JNIEnv* env, jclass jclass) {
    std::unique_ptr<WindowX11> instance = std::make_unique<WindowX11>(env);
    if (instance->init()) {
        return reinterpret_cast<jlong>(instance.release());
    }
    return 0;
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowX11_show
  (JNIEnv* env, jobject obj) {
      
    reinterpret_cast<jwm::WindowX11*>(jwm::classes::Native::fromJava(env, obj))->show();
}

extern "C" JNIEXPORT jint JNICALL Java_org_jetbrains_jwm_WindowX11_getLeft
  (JNIEnv* env, jobject obj) {

    return reinterpret_cast<jwm::WindowX11*>(jwm::classes::Native::fromJava(env, obj))->getLeft();
}

extern "C" JNIEXPORT jint JNICALL Java_org_jetbrains_jwm_WindowX11_getTop
  (JNIEnv* env, jobject obj) {
    
    return reinterpret_cast<jwm::WindowX11*>(jwm::classes::Native::fromJava(env, obj))->getTop();
}

extern "C" JNIEXPORT jint JNICALL Java_org_jetbrains_jwm_WindowX11_getWidth
  (JNIEnv* env, jobject obj) {
    
    return reinterpret_cast<jwm::WindowX11*>(jwm::classes::Native::fromJava(env, obj))->getWidth();
}

extern "C" JNIEXPORT jint JNICALL Java_org_jetbrains_jwm_WindowX11_getHeight
  (JNIEnv* env, jobject obj) {
    
    return reinterpret_cast<jwm::WindowX11*>(jwm::classes::Native::fromJava(env, obj))->getHeight();
}

extern "C" JNIEXPORT jfloat JNICALL Java_org_jetbrains_jwm_WindowX11_getScale
  (JNIEnv* env, jobject obj) {
   
    return reinterpret_cast<jwm::WindowX11*>(jwm::classes::Native::fromJava(env, obj))->getScale();
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowX11_move
  (JNIEnv* env, jobject obj, int left, int top) {
    reinterpret_cast<jwm::WindowX11*>(jwm::classes::Native::fromJava(env, obj))->move(left, top);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowX11_resize
  (JNIEnv* env, jobject obj, int width, int height) {
    reinterpret_cast<jwm::WindowX11*>(jwm::classes::Native::fromJava(env, obj))->resize(width, height);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowX11_requestFrame
  (JNIEnv* env, jobject obj) {
    
    reinterpret_cast<jwm::WindowX11*>(jwm::classes::Native::fromJava(env, obj))->requestRedraw();
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowX11__1nClose
  (JNIEnv* env, jobject obj) {
    reinterpret_cast<jwm::WindowX11*>(jwm::classes::Native::fromJava(env, obj))->close();
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowX11__1nStart
  (JNIEnv* env, jobject obj) {
    jwm::WindowX11* window = reinterpret_cast<jwm::WindowX11*>(jwm::classes::Native::fromJava(env, obj));
    window->runLoop();
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowX11__1nRunOnWindowThread
  (JNIEnv* env, jobject obj) {
}
