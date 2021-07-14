#include <ScreenWin32.hh>
#include <AppWin32.hh>
#include <impl/Library.hh>

jwm::ScreenWin32::ScreenWin32(JNIEnv *env, jwm::AppWin32 &app)
    : Screen(env),
      _app(app) {

}

jwm::ScreenWin32::~ScreenWin32() {

}

bool jwm::ScreenWin32::init() {
    printf("jwm::ScreenWin32::init()\n");
    return true;
}

int jwm::ScreenWin32::getX() const {
    return 0;
}

int jwm::ScreenWin32::getY() const {
    return 0;
}

int jwm::ScreenWin32::getWidth() const {
    return 1280;
}

int jwm::ScreenWin32::getHeight() const {
    return 720;
}

float jwm::ScreenWin32::getScale() const {
    return 1.0;
}

bool jwm::ScreenWin32::isPrimary() const {
    return true;
}

void jwm::ScreenWin32::close() {
    printf("jwm::ScreenWin32::close()\n");
}

// JNI

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_ScreenWin32__1nMake
        (JNIEnv* env, jclass jclass) {
    std::unique_ptr<jwm::ScreenWin32> instance(new jwm::ScreenWin32(env, jwm::AppWin32::getInstance()));
    if (instance->init())
        return reinterpret_cast<jlong>(instance.release());
    else
        return 0;
}

extern "C" JNIEXPORT jint JNICALL Java_org_jetbrains_jwm_ScreenWin32_getX
        (JNIEnv* env, jobject obj) {
    jwm::ScreenWin32* instance = reinterpret_cast<jwm::ScreenWin32*>(jwm::classes::Native::fromJava(env, obj));
    return instance->getX();
}

extern "C" JNIEXPORT jint JNICALL Java_org_jetbrains_jwm_ScreenWin32_getY
        (JNIEnv* env, jobject obj) {
    jwm::ScreenWin32* instance = reinterpret_cast<jwm::ScreenWin32*>(jwm::classes::Native::fromJava(env, obj));
    return instance->getY();
}

extern "C" JNIEXPORT jint JNICALL Java_org_jetbrains_jwm_ScreenWin32_getWidth
        (JNIEnv* env, jobject obj) {
    jwm::ScreenWin32* instance = reinterpret_cast<jwm::ScreenWin32*>(jwm::classes::Native::fromJava(env, obj));
    return instance->getWidth();
}

extern "C" JNIEXPORT jint JNICALL Java_org_jetbrains_jwm_ScreenWin32_getHeight
        (JNIEnv* env, jobject obj) {
    jwm::ScreenWin32* instance = reinterpret_cast<jwm::ScreenWin32*>(jwm::classes::Native::fromJava(env, obj));
    return instance->getHeight();
}

extern "C" JNIEXPORT jfloat JNICALL Java_org_jetbrains_jwm_ScreenWin32_getScale
        (JNIEnv* env, jobject obj) {
    jwm::ScreenWin32* instance = reinterpret_cast<jwm::ScreenWin32*>(jwm::classes::Native::fromJava(env, obj));
    return instance->getScale();
}

extern "C" JNIEXPORT jboolean JNICALL Java_org_jetbrains_jwm_ScreenWin32_isPrimary
        (JNIEnv* env, jobject obj) {
    jwm::ScreenWin32* instance = reinterpret_cast<jwm::ScreenWin32*>(jwm::classes::Native::fromJava(env, obj));
    return instance->isPrimary();
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_ScreenWin32__1nClose
        (JNIEnv* env, jobject obj) {
    jwm::ScreenWin32* instance = reinterpret_cast<jwm::ScreenWin32*>(jwm::classes::Native::fromJava(env, obj));
    instance->close();
}