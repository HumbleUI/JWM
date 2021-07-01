#include "WindowX11.hh"
#include <jni.h>
#include <memory>
#include "App.hh"
#include "impl/Library.hh"


using namespace jwm;


WindowX11::WindowX11(JNIEnv* env, WindowManagerX11& windowManager):
    jwm::Window(env),
    _windowManager(windowManager)
{

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
    return true;
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
      
    jwm::WindowX11* instance = reinterpret_cast<jwm::WindowX11*>(jwm::classes::Native::fromJava(env, obj));
    instance->show();
}

extern "C" JNIEXPORT jint JNICALL Java_org_jetbrains_jwm_WindowX11_getLeft
  (JNIEnv* env, jobject obj) {
    
      return 0;
}

extern "C" JNIEXPORT jint JNICALL Java_org_jetbrains_jwm_WindowX11_getTop
  (JNIEnv* env, jobject obj) {
    
      return 0;
}

extern "C" JNIEXPORT jint JNICALL Java_org_jetbrains_jwm_WindowX11_getWidth
  (JNIEnv* env, jobject obj) {
    
      return 0;
}

extern "C" JNIEXPORT jint JNICALL Java_org_jetbrains_jwm_WindowX11_getHeight
  (JNIEnv* env, jobject obj) {
    
      return 0;
}

extern "C" JNIEXPORT jfloat JNICALL Java_org_jetbrains_jwm_WindowX11_getScale
  (JNIEnv* env, jobject obj) {
   
      return 0;
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowX11_move
  (JNIEnv* env, jobject obj, int left, int top) {
    
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowX11_resize
  (JNIEnv* env, jobject obj, int width, int height) {
    
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowX11_requestFrame
  (JNIEnv* env, jobject obj) {
    
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowX11__1nClose
  (JNIEnv* env, jobject obj) {
    
}
