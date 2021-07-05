#include "WindowX11.hh"
#include <jni.h>
#include <memory>
#include "App.hh"
#include "impl/Library.hh"
#include <X11/Xresource.h>
#include <X11/Xatom.h>
#include <X11/extensions/sync.h>


using namespace jwm;


WindowX11::WindowX11(JNIEnv* env, WindowManagerX11& windowManager):
    jwm::Window(env),
    _windowManager(windowManager)
{
}

WindowX11::~WindowX11() {
    if (_x11Window) {
        _windowManager.unregisterWindow(this);
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
    XWindowAttributes xwa;
    XGetWindowAttributes(_windowManager.display, _x11Window, &xwa);
    return xwa.width;
}

int WindowX11::getHeight() {
    XWindowAttributes xwa;
    XGetWindowAttributes(_windowManager.display, _x11Window, &xwa);
    return xwa.height;
}

float WindowX11::getScale() {
    char *resourceString = XResourceManagerString(_windowManager.display);
    XrmDatabase db;
    XrmValue value;
    char *type = NULL;

    static struct once {
        once() {
            XrmInitialize();
        }
    } once;

    db = XrmGetStringDatabase(resourceString);

    if (resourceString) {
        if (XrmGetResource(db, "Xft.dpi", "String", &type, &value)) {
            if (value.addr) {
                return atof(value.addr) / 96.f;
            }
        }
    }

    return 1.f;
}

bool WindowX11::init()
{
    _x11Window = XCreateWindow(_windowManager.getDisplay(),
                               _windowManager.getScreen()->root,
                               0, 0,
                               800, 500,
                               0,
                               _windowManager.getVisualInfo()->depth,
                               InputOutput,
                               _windowManager.getVisualInfo()->visual,
                               CWColormap | CWEventMask | CWCursor,
                               &_windowManager.getSWA()
    );
    _windowManager.registerWindow(this);
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
    return true;
}

void WindowX11::move(int left, int top) {
    XMoveWindow(_windowManager.display, _x11Window, left, top);
}

void WindowX11::resize(int width, int height) {
    XResizeWindow(_windowManager.display, _x11Window, width, height);
}


void WindowX11::show() {
    XMapWindow(_windowManager.getDisplay(), _x11Window);
}

// JNI

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_WindowX11__1nMake
  (JNIEnv* env, jclass jclass) {
    std::unique_ptr<WindowX11> instance = std::make_unique<WindowX11>(env, jwm::app.getWindowManager());
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
    
}
