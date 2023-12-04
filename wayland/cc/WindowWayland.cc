#include "WindowWayland.hh"
#include <jni.h>
#include <memory>
#include <cstring>
#include "AppWayland.hh"
#include "impl/Library.hh"
#include "impl/JNILocal.hh"

using namespace jwm;


WindowWayland::WindowWayland(JNIEnv* env, WindowManagerWayland& windowManager):
    jwm::Window(env),
    _windowManager(windowManager)
{
}

WindowWayland::~WindowWayland() {
    close();
}


void WindowWayland::setTitle(const std::string& title) {
    // impl me : )
}

void WindowWayland::setTitlebarVisible(bool isVisible) {
    // impl me : )
}

// Closing is like... the exact same as being visible. WTH
void WindowWayland::close() {
    if (_waylandWindow) {
        _windowManager.unregisterWindow(this);
        wl_surface_destroy(_waylandWindow);
    }
    _waylandWindow = nullptr;
    if (xdgSurface) {
        xdg_surface_destroy(xdgSurface);
    }
    xdgSurface = nullptr;
    if (xdgToplevel) {
        xdg_toplevel_destroy(xdgToplevel);
    }
    xdgToplevel = nullptr;

}
void WindowWayland::maximize() {
    // impl me :) 
}

void WindowWayland::minimize() {
    // impl me : )
}

void WindowWayland::restore() {
    // impl me
}

void WindowWayland::setFullScreen(bool isFullScreen) {
    // impl me : )
}

bool WindowWayland::isFullScreen() {
    // impl me : )
    return false;
}

void WindowWayland::getDecorations(int& left, int& top, int& right, int& bottom) {
    // impl me : )
    left = 0;
    right = 0;
    top = 0;
    bottom = 0;

}

void WindowWayland::getContentPosition(int& posX, int& posY) {
    posX = 0;
    posY = 0;
    
}

int WindowWayland::getLeft() {
    int x, y;
    getContentPosition(x, y);
    return x;
}

int WindowWayland::getTop() {
    int x, y;
    getContentPosition(x, y);
    return y;
}

int WindowWayland::getWidth() {
    return _width;
}

int WindowWayland::getHeight() {
    return _height;
}

float WindowWayland::getScale() {
    return _scale;
}
bool WindowWayland::init() {
    return true;
}
void WindowWayland::show()
{
    _waylandWindow = wl_compositor_create_surface(_windowManager.compositor);
    wl_surface_listener surfaceListener = {
        .enter = WindowWayland::surfaceEnter,
        .leave = WindowWayland::surfaceLeave,
    #ifdef HAVE_WAYLAND_1_22
        .preferred_buffer_scale = WindowWayland::surfacePreferredBufferScale,
        .preferred_buffer_transform = WindowWayland::surfacePreferredBufferTransform
    #endif
    };
    wl_surface_add_listener(_waylandWindow, &surfaceListener, this);

    xdgSurface = xdg_wm_base_get_xdg_surface(_windowManager.xdgShell, _waylandWindow);
    xdg_surface_listener xdgSurfaceListener = {
        .configure = WindowWayland::xdgSurfaceConfigure
    };
    xdg_surface_add_listener(xdgSurface, &xdgSurfaceListener, this);

    xdgToplevel = xdg_surface_get_toplevel(xdgSurface);
    xdg_toplevel_listener xdgToplevelListener = {
        .configure = WindowWayland::xdgToplevelConfigure,
        .close = WindowWayland::xdgToplevelClose,
        .configure_bounds = WindowWayland::xdgToplevelConfigureBounds,
        .wm_capabilities = WindowWayland::xdgToplevelWmCapabilities
    };
    xdg_toplevel_add_listener(xdgToplevel, &xdgToplevelListener, this); 
    _windowManager.registerWindow(this);

}

// ???
void WindowWayland::recreate()
{
    close();
    init();
}

void WindowWayland::setVisible(bool isVisible) {
    if (_visible != isVisible) {
        _visible = isVisible;
        if (_visible) {
            show();
        } else {
            close();
        }
    }
}

void jwm::WindowWayland::setCursor(jwm::MouseCursor cursor) {
    if (auto wayCursor = _windowManager._cursors[static_cast<int>(cursor)]) {
        wl_surface_attach(_windowManager.cursorSurface, 
                wl_cursor_image_get_buffer(wayCursor),
                0, 0);
        wl_surface_commit(_windowManager.cursorSurface);
        // TODO: hotspots?
    } else {
        auto otherCursor = _windowManager._cursors[static_cast<int>(jwm::MouseCursor::ARROW)];
        wl_surface_attach(_windowManager.cursorSurface,
                wl_cursor_image_get_buffer(otherCursor), 0, 0);
        wl_surface_commit(_windowManager.cursorSurface);
    }
}

// what do???
void jwm::WindowWayland::surfaceEnter(void* data, wl_surface* surface, wl_output* output) {
    wl_output_listener listener = {
      .geometry = WindowWayland::outputGeometry,
      .mode = WindowWayland::outputMode,
      .done = WindowWayland::outputDone,
      .scale = WindowWayland::outputScale,
      .name = WindowWayland::outputName,
      .description = WindowWayland::outputDescription
    };
    wl_output_add_listener(output, &listener, data);
}
void jwm::WindowWayland::surfaceLeave(void* data, wl_surface* surface, wl_output* output) {}
void jwm::WindowWayland::surfacePreferredBufferScale(void* data, wl_surface* surface, int factor) {
    WindowWayland* self = (WindowWayland*) data;
    if (factor < 1) {
        return;
    }
    self->_scale = factor;
    // do I pinky promise here?
    // yes : )
    if (self->_layer) {
        self->_layer->resize(self->_width * factor, self->_height * factor); 
    }
    wl_surface_set_buffer_scale(surface, factor);
}
void jwm::WindowWayland::surfacePreferredBufferTransform(void* data, wl_surface* surface, uint32_t transform) {}

void jwm::WindowWayland::xdgSurfaceConfigure(void* data, xdg_surface* surface, uint32_t serial) {
    WindowWayland* self = (WindowWayland*) data;
    printf("hi guys");
    // Commit state
    if (self->_newWidth > 0 || self->_newHeight > 0) {
        int goodWidth = self->_width, goodHeight = self->_height;
        if (self->_newWidth > 0)
            goodWidth = self->_newWidth;
        if (self->_newHeight > 0)
            goodHeight = self->_newHeight;
        self->_adaptSize(goodWidth, goodHeight);
    }
    self->_newWidth = -1;
    self->_newHeight = -1;
    xdg_surface_ack_configure(surface, serial);
    if (self->_layer) {
        self->_layer->attachBuffer();
    }
}
void jwm::WindowWayland::xdgToplevelConfigure(void* data, xdg_toplevel* toplevel, int width, int height, wl_array* states) {
    WindowWayland* self = (WindowWayland*) data;
    if (width > 0) {
        self->_newWidth = width;
    }
    if (height > 0) {
        self->_newHeight = height;
    }
    // honestly idrc about the state
}
void jwm::WindowWayland::xdgToplevelClose(void* data, xdg_toplevel* toplevel) {
    // ???
    // Request close EVENTUALLY:TM:
}
void jwm::WindowWayland::xdgToplevelConfigureBounds(void* data, xdg_toplevel* toplevel, int width, int height) {
    WindowWayland* self = (WindowWayland*) data;
    if (width > 0) {
        self->_newWidth = width;
    }
    if (height > 0) {
        self->_newHeight = height;
    }
}
void jwm::WindowWayland::xdgToplevelWmCapabilities(void* data, xdg_toplevel* toplevel, wl_array* array) {
    // impl me : )
}
void jwm::WindowWayland::outputGeometry(void* data, wl_output* output, int x, int y, int pWidth, int pHeight,
        int subpixel, const char* make, const char* model, int transform) {}
void jwm::WindowWayland::outputMode(void* data, wl_output* output, uint32_t flags, int width, int height,
        int refresh) {}
void jwm::WindowWayland::outputDone(void* data, wl_output* output) {}
void jwm::WindowWayland::outputScale(void* data, wl_output* output, int factor) {
    WindowWayland* self = reinterpret_cast<WindowWayland*>(data);
    self->_scale = factor;
    if (self->_layer) {
        self->_layer->resize(self->_width * factor, self->_height * factor);
        if (self->_waylandWindow) {
            wl_surface_set_buffer_scale(self->_waylandWindow, factor);
        }
    }
}
void jwm::WindowWayland::outputName(void* data, wl_output* output, const char* name) {}
void jwm::WindowWayland::outputDescription(void* data, wl_output* output, const char* desc) {}
void jwm::WindowWayland::_adaptSize(int newWidth, int newHeight) {
    _width = newWidth;
    _height = newHeight;
    if (_layer) {
        _layer->resize(_width * _scale, _height * _scale);
    }
}
// JNI

extern "C" JNIEXPORT jlong JNICALL Java_io_github_humbleui_jwm_WindowWayland__1nMake
  (JNIEnv* env, jclass jclass) {
    std::unique_ptr<WindowWayland> instance = std::make_unique<WindowWayland>(env, jwm::app.getWindowManager());
    if (instance->init()) {
        return reinterpret_cast<jlong>(instance.release());
    }
    return 0;
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowWayland__1nSetVisible
  (JNIEnv* env, jobject obj, jboolean isVisible) {
      
    reinterpret_cast<jwm::WindowWayland*>(jwm::classes::Native::fromJava(env, obj))->setVisible(isVisible);
}

extern "C" JNIEXPORT jobject JNICALL Java_io_github_humbleui_jwm_WindowWayland__1nGetWindowRect
  (JNIEnv* env, jobject obj) {
    jwm::WindowWayland* instance = reinterpret_cast<jwm::WindowWayland*>(jwm::classes::Native::fromJava(env, obj));
    int left, top, right, bottom;
    instance->getDecorations(left, top, right, bottom);
    int x, y;
    instance->getContentPosition(x, y);
    return jwm::classes::IRect::toJavaXYWH(
      env,
      x-left,
      y-top,
      instance->getWidth()+left+right,
      instance->getHeight()+top+bottom
    );
}

extern "C" JNIEXPORT jobject JNICALL Java_io_github_humbleui_jwm_WindowWayland__1nGetContentRect
  (JNIEnv* env, jobject obj) {
    jwm::WindowWayland* instance = reinterpret_cast<jwm::WindowWayland*>(jwm::classes::Native::fromJava(env, obj));
    int left, top, right, bottom;
    instance->getDecorations(left, top, right, bottom);
    return jwm::classes::IRect::toJavaXYWH(
      env,
      left,
      top,
      instance->getWidth(),
      instance->getHeight()
    );
}


extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowWayland__1nRequestFrame
  (JNIEnv* env, jobject obj) {
    jwm::WindowWayland* instance = reinterpret_cast<jwm::WindowWayland*>(jwm::classes::Native::fromJava(env, obj));
    instance->requestRedraw();
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowWayland__1nMaximize
  (JNIEnv* env, jobject obj) {
    reinterpret_cast<jwm::WindowWayland*>(jwm::classes::Native::fromJava(env, obj))->maximize();
}


extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowWayland__1nMinimize
  (JNIEnv* env, jobject obj) {
    reinterpret_cast<jwm::WindowWayland*>(jwm::classes::Native::fromJava(env, obj))->minimize();
}


extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowWayland__1nRestore
  (JNIEnv* env, jobject obj) {
    reinterpret_cast<jwm::WindowWayland*>(jwm::classes::Native::fromJava(env, obj))->restore();
}


extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowWayland__1nClose
        (JNIEnv* env, jobject obj) {
    jwm::WindowWayland* instance = reinterpret_cast<jwm::WindowWayland*>(jwm::classes::Native::fromJava(env, obj));
    instance->close();
}
extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowWayland__1nSetTitle
        (JNIEnv* env, jobject obj, jbyteArray title) {
    jwm::WindowWayland* instance = reinterpret_cast<jwm::WindowWayland*>(jwm::classes::Native::fromJava(env, obj));

    jbyte* bytes = env->GetByteArrayElements(title, nullptr);
    std::string titleS = { bytes, bytes + env->GetArrayLength(title) };
    env->ReleaseByteArrayElements(title, bytes, 0);

    instance->setTitle(titleS);
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowWayland__1nSetTitlebarVisible
        (JNIEnv* env, jobject obj, jboolean isVisible) {
    jwm::WindowWayland* instance = reinterpret_cast<jwm::WindowWayland*>(jwm::classes::Native::fromJava(env, obj));
    instance->setTitlebarVisible(isVisible);
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowWayland__1nSetMouseCursor
        (JNIEnv* env, jobject obj, jint idx) {
    jwm::WindowWayland* instance = reinterpret_cast<jwm::WindowWayland*>(jwm::classes::Native::fromJava(env, obj));

    instance->setCursor(static_cast<jwm::MouseCursor>(idx));
}

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowWayland__1nSetFullScreen
        (JNIEnv* env, jobject obj, jboolean isFullScreen) {
    jwm::WindowWayland* instance = reinterpret_cast<jwm::WindowWayland*>(jwm::classes::Native::fromJava(env, obj));
    instance->setFullScreen(isFullScreen);
}

extern "C" JNIEXPORT jboolean JNICALL Java_io_github_humbleui_jwm_WindowWayland__1nIsFullScreen
        (JNIEnv* env, jobject obj) {
    jwm::WindowWayland* instance = reinterpret_cast<jwm::WindowWayland*>(jwm::classes::Native::fromJava(env, obj));
    return instance->isFullScreen();
}

extern "C" JNIEXPORT jfloat JNICALL Java_io_github_humbleui_jwm_WindowWayland__1nGetScale
        (JNIEnv* env, jobject obj) {
    jwm::WindowWayland* instance = reinterpret_cast<jwm::WindowWayland*>(jwm::classes::Native::fromJava(env, obj));
    return instance->getScale();
}
