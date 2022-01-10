#include "WindowX11.hh"
#include <jni.h>
#include <memory>
#include "AppX11.hh"
#include "impl/Library.hh"
#include "impl/JNILocal.hh"
#include <X11/Xatom.h>
#include <X11/extensions/sync.h>


using namespace jwm;


WindowX11::WindowX11(JNIEnv* env, WindowManagerX11& windowManager):
    jwm::Window(env),
    _windowManager(windowManager)
{
}

WindowX11::~WindowX11() {
    close();
}

void WindowX11::setTitle(const std::string& title) {
    XChangeProperty(_windowManager.getDisplay(),
                    _x11Window,
                    _windowManager.getAtoms()._NET_WM_NAME,
                    _windowManager.getAtoms().UTF8_STRING,
                    8,
                    PropModeReplace,
                    reinterpret_cast<const unsigned char*>(title.c_str()),
                    title.length());
}

void WindowX11::close() {
    if (_x11Window) {
        _windowManager.unregisterWindow(this);
        XDestroyWindow(_windowManager.display, _x11Window);
        _x11Window = 0;
    }
}
void WindowX11::_xSendEventToWM(Atom atom, long a, long b, long c, long d, long e) const {
    XEvent event = { 0 };
    event.type = ClientMessage;
    event.xclient.window = _x11Window;
    event.xclient.format = 32; // data is 32-bit longs
    event.xclient.message_type = atom;
    event.xclient.data.l[0] = a;
    event.xclient.data.l[1] = b;
    event.xclient.data.l[2] = c;
    event.xclient.data.l[3] = d;
    event.xclient.data.l[4] = e;

    XSendEvent(_windowManager.display,
               DefaultRootWindow(_windowManager.display),
               False,
               SubstructureNotifyMask | SubstructureRedirectMask,
               &event);
}
unsigned long WindowX11::_xGetWindowProperty(Atom property, Atom type, unsigned char** value) const {
    Atom actualType;
    int actualFormat;
    unsigned long itemCount, bytesAfter;

    XGetWindowProperty(_windowManager.display,
                       _x11Window,
                       property,
                       0,
                       std::numeric_limits<long>::max(),
                       false,
                       type,
                       &actualType,
                       &actualFormat,
                       &itemCount,
                       &bytesAfter,
                       value);

    return itemCount;
}

void WindowX11::maximize() {
    XWindowAttributes wa;
    XGetWindowAttributes(_windowManager.display, _x11Window, &wa);

    if (wa.map_state == IsViewable) {
        _xSendEventToWM(_windowManager._atoms._NET_WM_STATE,
                        1,
                        _windowManager._atoms._NET_WM_STATE_MAXIMIZED_HORZ,
                        _windowManager._atoms._NET_WM_STATE_MAXIMIZED_VERT,
                        0,
                        0);
    } else {
        Atom* states = nullptr;
        unsigned long count = _xGetWindowProperty(_windowManager._atoms._NET_WM_STATE,
                                                  XA_ATOM,
                                                  reinterpret_cast<unsigned char**>(&states));


        Atom missing[2] = {
            _windowManager._atoms._NET_WM_STATE_MAXIMIZED_VERT,
            _windowManager._atoms._NET_WM_STATE_MAXIMIZED_HORZ
        };
        unsigned long missingCount = 2;

        for (unsigned long i = 0;  i < count;  i++)
        {
            for (unsigned long j = 0;  j < missingCount;  j++)
            {
                if (states[i] == missing[j])
                {
                    missing[j] = missing[missingCount - 1];
                    missingCount--;
                }
            }
        }

        if (states)
            XFree(states);

        if (!missingCount)
            return;

        XChangeProperty(_windowManager.display,
                        _x11Window,
                        _windowManager._atoms._NET_WM_STATE,
                        XA_ATOM,
                        32,
                        PropModeAppend,
                        (unsigned char*) missing,
                        missingCount);
    }
    XFlush(_windowManager.display);
}

void WindowX11::minimize() {
    XIconifyWindow(_windowManager.display, _x11Window, 0);
}

void WindowX11::restore() {
    if (_windowManager._atoms._NET_WM_STATE &&
        _windowManager._atoms._NET_WM_STATE_MAXIMIZED_VERT &&
        _windowManager._atoms._NET_WM_STATE_MAXIMIZED_HORZ) {
            _xSendEventToWM(_windowManager._atoms._NET_WM_STATE,
                            0,
                            _windowManager._atoms._NET_WM_STATE_MAXIMIZED_VERT,
                            _windowManager._atoms._NET_WM_STATE_MAXIMIZED_HORZ,
                            1,
                            0);
    }
}

void WindowX11::getPosition(int& posX, int& posY) {
    int x, y;
    ::Window child;
    XWindowAttributes xwa;
    XTranslateCoordinates(_windowManager.display,
                          _x11Window,
                          XRootWindow(_windowManager.display, 0),
                          0, 0,
                          &x, &y,
                          &child);
    XGetWindowAttributes(_windowManager.display, _x11Window, &xwa);
    
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
    _x11Window = XCreateWindow(_windowManager.getDisplay(),
                               _windowManager.getScreen()->root,
                               0, 0,
                               800, 500,
                               0,
                               _windowManager.getX11VisualDepth(),
                               InputOutput,
                               _windowManager.getX11Visual(),
                               CWColormap | CWEventMask | CWCursor,
                               &_windowManager.getSWA()
    );

    // tell X11 we want to handle close button
    XSetWMProtocols(_windowManager.getDisplay(),
                    _x11Window,
                    &_windowManager.getAtoms().WM_DELETE_WINDOW,
                    WindowManagerX11::Atoms::PROTOCOL_COUNT);

    // IC
    {
        _ic = XCreateIC(_windowManager.getIM(),
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
        _xsyncRequestCounter.counter = XSyncCreateCounter(_windowManager.getDisplay(), value);
        XChangeProperty(_windowManager.getDisplay(),
                        _x11Window,
                        _windowManager.getAtoms()._NET_WM_SYNC_REQUEST_COUNTER,
                        XA_CARDINAL,
                        32,
                        PropModeReplace,
                        (const unsigned char*)&_xsyncRequestCounter.counter, 1);
        
    }
    _windowManager.registerWindow(this);
    return true;
}

void WindowX11::move(int left, int top) {
    XMoveWindow(_windowManager.display, _x11Window, left, top);
}

void WindowX11::resize(int width, int height) {
    _width = width;
    _height = height;
    XResizeWindow(_windowManager.display, _x11Window, width, height);
    jwm::JNILocal<jobject> eventWindowResize(app.getJniEnv(), classes::EventWindowResize::make(app.getJniEnv(), width, height, width, height));
    dispatch(eventWindowResize.get());
}


void WindowX11::setVisible(bool isVisible) {
    if (isVisible) {
        XMapWindow(_windowManager.getDisplay(), _x11Window);
    } else {
        XUnmapWindow(_windowManager.getDisplay(), _x11Window);
    }
}

const ScreenInfo& WindowX11::getScreen() {
    // in X11, there's no straightforward way to get screen of window.
    // instead, we should do it manually using center point of the window and calculating which monitor this point
    // belongs to.

    int centerX = getLeft() + getWidth()  / 2;
    int centerY = getTop()  + getHeight() / 2;
    for (auto& screen : jwm::app.getScreens()) {
        if (screen.bounds.isPointInside(centerX, centerY)) {
            return screen;
        }
    }
    return *jwm::app.getScreens().begin();
}

void jwm::WindowX11::setCursor(jwm::MouseCursor cursor) {
    if (auto x11Cursor = _windowManager._cursors[static_cast<int>(cursor)]) {
        XDefineCursor(_windowManager.display, _x11Window, x11Cursor);
    } else {
        XUndefineCursor(_windowManager.display, _x11Window);
    }
}

// JNI

extern "C" JNIEXPORT jlong JNICALL Java_io_github_humbleui_jwm_WindowX11__1nMake
  (JNIEnv* env, jclass jclass) {
    std::unique_ptr<WindowX11> instance = std::make_unique<WindowX11>(env, jwm::app.getWindowManager());
    if (instance->init()) {
        return reinterpret_cast<jlong>(instance.release());
    }
    return 0;
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowX11__1nSetVisible
  (JNIEnv* env, jobject obj, jboolean isVisible) {
      
    reinterpret_cast<jwm::WindowX11*>(jwm::classes::Native::fromJava(env, obj))->setVisible(isVisible);
}

extern "C" JNIEXPORT jobject JNICALL Java_io_github_humbleui_jwm_WindowX11__1nGetWindowRect
  (JNIEnv* env, jobject obj) {
    jwm::WindowX11* instance = reinterpret_cast<jwm::WindowX11*>(jwm::classes::Native::fromJava(env, obj));
    return jwm::classes::IRect::toJavaXYWH(
      env,
      instance->getLeft(),
      instance->getTop(),
      instance->getWidth(),
      instance->getHeight()
    );
}

extern "C" JNIEXPORT jobject JNICALL Java_io_github_humbleui_jwm_WindowX11__1nGetContentRect
  (JNIEnv* env, jobject obj) {
    jwm::WindowX11* instance = reinterpret_cast<jwm::WindowX11*>(jwm::classes::Native::fromJava(env, obj));
    // TODO https://github.com/HumbleUI/JWM/issues/109
    return jwm::classes::IRect::toJavaXYWH(
      env,
      0,
      0,
      instance->getWidth(),
      instance->getHeight()
    );
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowX11__1nSetWindowPosition
        (JNIEnv* env, jobject obj, int left, int top) {
    jwm::WindowX11* instance = reinterpret_cast<jwm::WindowX11*>(jwm::classes::Native::fromJava(env, obj));
    instance->move(left, top);
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowX11__1nSetWindowSize
        (JNIEnv* env, jobject obj, int width, int height) {
    jwm::WindowX11* instance = reinterpret_cast<jwm::WindowX11*>(jwm::classes::Native::fromJava(env, obj));
    // TODO https://github.com/HumbleUI/JWM/issues/109
    instance->resize(width, height);
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowX11__1nSetContentSize
        (JNIEnv* env, jobject obj, int width, int height) {
    jwm::WindowX11* instance = reinterpret_cast<jwm::WindowX11*>(jwm::classes::Native::fromJava(env, obj));
    instance->resize(width, height);
}

extern "C" JNIEXPORT jobject JNICALL Java_io_github_humbleui_jwm_WindowX11__1nGetScreen
  (JNIEnv* env, jobject obj) {
    jwm::WindowX11* instance = reinterpret_cast<jwm::WindowX11*>(jwm::classes::Native::fromJava(env, obj));
    return instance->getScreen().asJavaObject(env); 
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowX11__1nRequestFrame
  (JNIEnv* env, jobject obj) {
    jwm::WindowX11* instance = reinterpret_cast<jwm::WindowX11*>(jwm::classes::Native::fromJava(env, obj));
    instance->requestRedraw();
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowX11__1nMaximize
  (JNIEnv* env, jobject obj) {
    reinterpret_cast<jwm::WindowX11*>(jwm::classes::Native::fromJava(env, obj))->maximize();
}


extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowX11__1nMinimize
  (JNIEnv* env, jobject obj) {
    reinterpret_cast<jwm::WindowX11*>(jwm::classes::Native::fromJava(env, obj))->minimize();
}


extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowX11__1nRestore
  (JNIEnv* env, jobject obj) {
    reinterpret_cast<jwm::WindowX11*>(jwm::classes::Native::fromJava(env, obj))->restore();
}


extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowX11__1nClose
        (JNIEnv* env, jobject obj) {
    jwm::WindowX11* instance = reinterpret_cast<jwm::WindowX11*>(jwm::classes::Native::fromJava(env, obj));
    instance->close();
}
extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowX11__1nSetTitle
        (JNIEnv* env, jobject obj, jbyteArray title) {
    jwm::WindowX11* instance = reinterpret_cast<jwm::WindowX11*>(jwm::classes::Native::fromJava(env, obj));

    jbyte* bytes = env->GetByteArrayElements(title, nullptr);
    std::string titleS = { bytes, bytes + env->GetArrayLength(title) };
    env->ReleaseByteArrayElements(title, bytes, 0);

    instance->setTitle(titleS);
}
extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowX11__1nSetMouseCursor
        (JNIEnv* env, jobject obj, jint idx) {
    jwm::WindowX11* instance = reinterpret_cast<jwm::WindowX11*>(jwm::classes::Native::fromJava(env, obj));

    instance->setCursor(static_cast<jwm::MouseCursor>(idx));
}