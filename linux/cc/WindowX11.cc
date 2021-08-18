#include "WindowX11.hh"
#include <jni.h>
#include <memory>
#include "AppX11.hh"
#include "impl/Library.hh"
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

void WindowX11::close() {
    if (_x11Window) {
        _windowManager.unregisterWindow(this);
        XDestroyWindow(_windowManager.display, _x11Window);
        _x11Window = 0;
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
                               _windowManager.getVisualInfo()->depth,
                               InputOutput,
                               _windowManager.getVisualInfo()->visual,
                               CWColormap | CWEventMask | CWCursor,
                               &_windowManager.getSWA()
    );
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

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowX11__1nShow
  (JNIEnv* env, jobject obj) {
      
    reinterpret_cast<jwm::WindowX11*>(jwm::classes::Native::fromJava(env, obj))->show();
}

extern "C" JNIEXPORT jobject JNICALL Java_org_jetbrains_jwm_WindowX11__1nGetWindowRect
  (JNIEnv* env, jobject obj) {
    jwm::WindowX11* instance = reinterpret_cast<jwm::WindowX11*>(jwm::classes::Native::fromJava(env, obj));
    return jwm::classes::UIRect::toJavaXYWH(
      env,
      instance->getLeft(),
      instance->getTop(),
      instance->getWidth(),
      instance->getHeight()
    );
}

extern "C" JNIEXPORT jobject JNICALL Java_org_jetbrains_jwm_WindowX11__1nGetContentRect
  (JNIEnv* env, jobject obj) {
    jwm::WindowX11* instance = reinterpret_cast<jwm::WindowX11*>(jwm::classes::Native::fromJava(env, obj));
    // TODO https://github.com/JetBrains/JWM/issues/109
    return jwm::classes::UIRect::toJavaXYWH(
      env,
      0,
      0,
      instance->getWidth(),
      instance->getHeight()
    );
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowX11__1nSetWindowPosition
        (JNIEnv* env, jobject obj, int left, int top) {
    jwm::WindowX11* instance = reinterpret_cast<jwm::WindowX11*>(jwm::classes::Native::fromJava(env, obj));
    instance->move(left, top);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowX11__1nSetWindowSize
        (JNIEnv* env, jobject obj, int width, int height) {
    jwm::WindowX11* instance = reinterpret_cast<jwm::WindowX11*>(jwm::classes::Native::fromJava(env, obj));
    // TODO https://github.com/JetBrains/JWM/issues/109
    instance->resize(width, height);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowX11__1nSetContentSize
        (JNIEnv* env, jobject obj, int width, int height) {
    jwm::WindowX11* instance = reinterpret_cast<jwm::WindowX11*>(jwm::classes::Native::fromJava(env, obj));
    instance->resize(width, height);
}

extern "C" JNIEXPORT jfloat JNICALL Java_org_jetbrains_jwm_WindowX11__1nGetScale
  (JNIEnv* env, jobject obj) {
   
    return reinterpret_cast<jwm::WindowX11*>(jwm::classes::Native::fromJava(env, obj))->getScale();
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowX11__1nRequestFrame
  (JNIEnv* env, jobject obj) {
    
    reinterpret_cast<jwm::WindowX11*>(jwm::classes::Native::fromJava(env, obj))->requestRedraw();
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowX11__1nClose
  (JNIEnv* env, jobject obj) {
    reinterpret_cast<jwm::WindowX11*>(jwm::classes::Native::fromJava(env, obj))->close();
}
