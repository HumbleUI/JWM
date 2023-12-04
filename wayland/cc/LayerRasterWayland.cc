// JNI

#include <jni.h>
#include "impl/Library.hh"
#include "impl/RefCounted.hh"
#include "WindowWayland.hh"
#include "ShmPool.hh"
#include <optional>

namespace jwm {
    class LayerRaster: public RefCounted, public ILayer {
    public:
        WindowWayland* fWindow;
        size_t _width = 0, _height = 0;
        wl_buffer* _buffer = nullptr;
        uint8_t* _imageData = nullptr;
        ShmPool* _pool = nullptr;
        VSync _vsync = VSYNC_ENABLED;

        LayerRaster() = default;
        virtual ~LayerRaster() = default;

        void attach(WindowWayland* window) {
            fWindow = jwm::ref(window);
            fWindow->setLayer(this);
            // a default size : )
            resize(100, 100);
        }

        void resize(int width, int height) override {
            wl_display* d = fWindow->_windowManager.display;
            _width = width;
            _height = height;
            int stride = width * sizeof(uint32_t);
            int bufSize = stride * height * 2;
            // TODO: better pool impl
            if (_pool) {
                delete _pool;
            }
            _pool = new ShmPool(fWindow->_windowManager.shm, bufSize);
            if (_buffer) {
                wl_buffer_destroy(_buffer);
                _imageData = nullptr;
            }

            _pool->grow(bufSize);
            // LSBFirst means Little endian : )
            // This will highly likely cause issues : )
            auto buf = _pool->createBuffer(0, width, height, stride, WL_SHM_FORMAT_ABGR8888);
         
            _buffer = buf.first;
            _imageData = buf.second;


        }

        const void* getPixelsPtr() const {
            return _imageData;
        }

        int getRowBytes() const {

            return _width * sizeof(uint32_t);
        }

        void swapBuffers() {
            // : )
            if (fWindow->_waylandWindow)
                wl_surface_damage_buffer(fWindow->_waylandWindow, 0, 0, UINT32_MAX, UINT32_MAX);
        }

        void close() override {
            if (_buffer) {
                wl_buffer_destroy(_buffer);
                _buffer = nullptr;
            }
            // ???
            delete _pool;
            jwm::unref(&fWindow);
        }

        void makeCurrentForced() override {
            ILayer::makeCurrentForced();
        }
        
        void setVsyncMode(VSync v) override {
            // srsly, why do I need this
            _vsync = v;
        }
    };
}


extern "C" JNIEXPORT jlong JNICALL Java_io_github_humbleui_jwm_LayerRaster__1nMake
        (JNIEnv* env, jclass jclass) {
    jwm::LayerRaster* instance = new jwm::LayerRaster;
    return reinterpret_cast<jlong>(instance);
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_LayerRaster__1nAttach
        (JNIEnv* env, jobject obj, jobject windowObj) {
    jwm::LayerRaster* instance = reinterpret_cast<jwm::LayerRaster*>(jwm::classes::Native::fromJava(env, obj));
    jwm::WindowWayland* window = reinterpret_cast<jwm::WindowWayland*>(jwm::classes::Native::fromJava(env, windowObj));
    instance->attach(window);
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_LayerRaster__1nReconfigure
        (JNIEnv* env, jobject obj) {
    
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_LayerRaster__1nResize
        (JNIEnv* env, jobject obj, jint width, jint height) {
    jwm::LayerRaster* instance = reinterpret_cast<jwm::LayerRaster*>(jwm::classes::Native::fromJava(env, obj));
    instance->resize(width, height);
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_LayerRaster__1nSwapBuffers
        (JNIEnv* env, jobject obj) {
    jwm::LayerRaster* instance = reinterpret_cast<jwm::LayerRaster*>(jwm::classes::Native::fromJava(env, obj));
    instance->swapBuffers();
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_LayerRaster__1nClose
        (JNIEnv* env, jobject obj) {
    jwm::LayerRaster* instance = reinterpret_cast<jwm::LayerRaster*>(jwm::classes::Native::fromJava(env, obj));
    instance->close();
}

extern "C" JNIEXPORT jlong JNICALL Java_io_github_humbleui_jwm_LayerRaster__1nGetPixelsPtr
        (JNIEnv* env, jobject obj) {
    jwm::LayerRaster* instance = reinterpret_cast<jwm::LayerRaster*>(jwm::classes::Native::fromJava(env, obj));
    return reinterpret_cast<jlong>(instance->getPixelsPtr());
}

extern "C" JNIEXPORT jlong JNICALL Java_io_github_humbleui_jwm_LayerRaster__1nGetRowBytes
        (JNIEnv* env, jobject obj) {
    jwm::LayerRaster* instance = reinterpret_cast<jwm::LayerRaster*>(jwm::classes::Native::fromJava(env, obj));
    return static_cast<jint>(instance->getRowBytes());
} 
