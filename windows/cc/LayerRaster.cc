#include <LayerRaster.hh>
#include <AppWin32.hh>
#include <WindowWin32.hh>
#include <impl/Library.hh>
#include <jni.h>

void jwm::LayerRaster::attach(WindowWin32 *window) {
    assert(!_windowWin32);

    AppWin32& app = AppWin32::getInstance();

    if (!window) {
        app.sendError("Passed null WindowWin32 object to attach");
        return;
    }

    if (window->testFlag(WindowWin32::Flag::HasAttachedLayer)) {
        app.sendError("Window already has attached layer. Cannot re-attach.");
        return;
    }

    if (window->testFlag(WindowWin32::Flag::RecreateForNextLayer)) {
        window->removeFlag(WindowWin32::Flag::RecreateForNextLayer);
        window->recreate();
    }

    _windowWin32 = jwm::ref(window);
    _windowWin32->setFlag(WindowWin32::Flag::HasAttachedLayer);
    _windowWin32->setFlag(WindowWin32::Flag::HasLayerRaster);

    _hDC = GetDC(_windowWin32->getHWnd());

    _callbackID = _windowWin32->addEventListener([this](WindowWin32::Event event){
        switch (event) {
            case WindowWin32::Event::SwapBuffers:
                swapBuffers();
                break;
            case WindowWin32::Event::EnableVsync:
                vsync(true);
                break;
            case WindowWin32::Event::DisableVsync:
                vsync(false);
                break;
            default:
                return;
        }
    });

    UIRect rect = _windowWin32->getContentRect();
    int width = rect.getWidth();
    int height = rect.getHeight();
    resize(width, height);
}

void jwm::LayerRaster::resize(int width, int height) {
    _width = width > 1? width: 1;
    _height = height > 1? height: 1;

    size_t bitmapSize = sizeof(BITMAPINFO) + _width * _height * sizeof(uint32_t);

    _bitmapMemory.resize(bitmapSize);
    _bitmapInfo = reinterpret_cast<BITMAPINFO*>(_bitmapMemory.data());

    ZeroMemory(_bitmapInfo, sizeof(BITMAPINFO));

    _bitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    _bitmapInfo->bmiHeader.biWidth = _width;
    _bitmapInfo->bmiHeader.biHeight = -_height; // negative means top-down bitmap. Skia draws top-down.
    _bitmapInfo->bmiHeader.biPlanes = 1;
    _bitmapInfo->bmiHeader.biBitCount = 32;
    _bitmapInfo->bmiHeader.biCompression = BI_RGB;
}

void jwm::LayerRaster::swapBuffers() {
    StretchDIBits(
        _hDC,
        0, 0, _width, _height,
        0, 0, _width, _height,
        _bitmapInfo->bmiColors, _bitmapInfo,
        DIB_RGB_COLORS, SRCCOPY
    );
}

void jwm::LayerRaster::close() {
    _releaseInternal();
}

void jwm::LayerRaster::vsync(bool enable) {

}

void jwm::LayerRaster::_releaseInternal() {
    _bitmapInfo = nullptr;
    _bitmapMemory.clear();

    if (_hDC) {
        ReleaseDC(_windowWin32->getHWnd(), _hDC);
        _hDC = nullptr;
    }

    if (_windowWin32) {
        _windowWin32->removeFlag(WindowWin32::Flag::HasAttachedLayer);
        _windowWin32->removeFlag(WindowWin32::Flag::HasLayerRaster);
        _windowWin32->removeEventListener(_callbackID);
        jwm::unref(&_windowWin32);
    }
}

void *jwm::LayerRaster::getPixelsPtr() const {
    return _bitmapInfo? _bitmapInfo->bmiColors: nullptr;
}

int jwm::LayerRaster::getRowBytes() const {
    return static_cast<int>(_width * sizeof(uint32_t));
}

// JNI

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_LayerRaster__1nMake
        (JNIEnv* env, jclass jclass) {
    jwm::LayerRaster* instance = new jwm::LayerRaster();
    return reinterpret_cast<jlong>(instance);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_LayerRaster__1nAttach
        (JNIEnv* env, jobject obj, jobject windowObj) {
    jwm::LayerRaster* instance = reinterpret_cast<jwm::LayerRaster*>(jwm::classes::Native::fromJava(env, obj));
    jwm::WindowWin32* window = reinterpret_cast<jwm::WindowWin32*>(jwm::classes::Native::fromJava(env, windowObj));
    instance->attach(window);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_LayerRaster__1nReconfigure
        (JNIEnv* env, jobject obj, jint width, jint height) {
    jwm::LayerRaster* instance = reinterpret_cast<jwm::LayerRaster*>(jwm::classes::Native::fromJava(env, obj));
    instance->resize(width, height);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_LayerRaster__1nResize
        (JNIEnv* env, jobject obj, jint width, jint height) {
    jwm::LayerRaster* instance = reinterpret_cast<jwm::LayerRaster*>(jwm::classes::Native::fromJava(env, obj));
    instance->resize(width, height);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_LayerRaster__1nSwapBuffers
        (JNIEnv* env, jobject obj) {
    //jwm::LayerRaster* instance = reinterpret_cast<jwm::LayerRaster*>(jwm::classes::Native::fromJava(env, obj));
    //instance->swapBuffers();
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_LayerRaster__1nClose
        (JNIEnv* env, jobject obj) {
    jwm::LayerRaster* instance = reinterpret_cast<jwm::LayerRaster*>(jwm::classes::Native::fromJava(env, obj));
    instance->close();
}

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_LayerRaster_getPixelsPtr
        (JNIEnv* env, jobject obj) {
    jwm::LayerRaster* instance = reinterpret_cast<jwm::LayerRaster*>(jwm::classes::Native::fromJava(env, obj));
    return reinterpret_cast<jlong>(instance->getPixelsPtr());
}

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_LayerRaster_getRowBytes
        (JNIEnv* env, jobject obj) {
    jwm::LayerRaster* instance = reinterpret_cast<jwm::LayerRaster*>(jwm::classes::Native::fromJava(env, obj));
    return static_cast<jint>(instance->getRowBytes());
}