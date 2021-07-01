#include "WindowX11.hh"
#include <jni.h>
#include <memory>


using namespace jwm;


WindowX11::WindowX11(JNIEnv* env, WindowManagerX11& windowManager):
    jwm::Window(env),
    mWindowManager(windowManager)
{

}



bool WindowX11::init()
{
    return true;
}
// JNI

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_WindowX11__1nMake
  (JNIEnv* env, jclass jclass) {
    static WindowManagerX11 wm;
    std::unique_ptr<WindowX11> instance = std::make_unique<WindowX11>(env, wm);
    if (instance->init()) {
        return reinterpret_cast<jlong>(instance.release());
    }
    return 0;
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowX11_show
  (JNIEnv* env, jobject obj) {
    
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
