#include "WindowWayland.hh"
#include <jni.h>
#include <memory>
#include <cstring>
#include "AppWayland.hh"
#include "impl/Library.hh"
#include "impl/JNILocal.hh"
#include <locale>
#include <codecvt>

using namespace jwm;


wl_surface_listener WindowWayland::_surfaceListener = {
    .enter = WindowWayland::surfaceEnter,
    .leave = WindowWayland::surfaceLeave,
#ifdef HAVE_WAYLAND_1_22
    .preferred_buffer_scale = WindowWayland::surfacePreferredBufferScale,
    .preferred_buffer_transform = WindowWayland::surfacePreferredBufferTransform
#endif
};
 
        
wl_output_listener WindowWayland::_outputListener = {
  .geometry = WindowWayland::outputGeometry,
  .mode = WindowWayland::outputMode,
  .done = WindowWayland::outputDone,
  .scale = WindowWayland::outputScale,
  .name = WindowWayland::outputName,
  .description = WindowWayland::outputDescription
};
libdecor_frame_interface WindowWayland::_libdecorFrameInterface = {
    .configure = WindowWayland::decorFrameConfigure,
    .close = WindowWayland::decorFrameClose,
    .commit = WindowWayland::decorFrameCommit,
    .dismiss_popup = WindowWayland::decorFrameDismissPopup
};

const char* WindowWayland::_windowTag = "WindowWayland";

WindowWayland::WindowWayland(JNIEnv* env, WindowManagerWayland& windowManager):
    jwm::Window(env),
    _windowManager(windowManager),
    _title("")

{
    _makeCursors();
}

WindowWayland::~WindowWayland() {
    // TODO: close gets called twice?
    // close();
}


void WindowWayland::setTitle(const std::string& title) {
    _title = title;
    if (_frame)
        libdecor_frame_set_title(_frame, _title.c_str());
}

void WindowWayland::setTitlebarVisible(bool isVisible) {
    _titlebarVisible = isVisible;
    if (_frame)
        libdecor_frame_set_visibility(_frame, isVisible);
}

void WindowWayland::close() {
    _windowManager.unregisterWindow(this);
    hide();
    if (_waylandWindow) {
        wl_surface_destroy(_waylandWindow);
    }
    _waylandWindow = nullptr;
    // TODO: more destruction!
}
void WindowWayland::hide() {
    _visible = false;
    if (_layer) {
        _layer->detach();
    }
    if (_frame) {
        libdecor_frame_unref(_frame);
    }
    _frame = nullptr;
    _configured = false;
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
    return _fullscreen;
}

void WindowWayland::setLayer(ILayerWayland* layer) {
    _layer = layer;

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

bool WindowWayland::resize(int width, int height) {
    if (width < 0 || height < 0)
        return false;
    // don't allow size to be set if currently tiled
    if (!_floating)
        return false;
    // Width and height are in absolute pixel units, and wayland will
    // complain if you try to set a width/height that isn't a multiple of scale.
    if ((width % _scale) != 0 || (height % _scale) != 0)
        return false;

    // TODO: adapting size while visible causes issues?
    // I've noticed the frame not updating
    if (_visible)
        return false;
    _width = width / _scale;
    _height = height / _scale;
    /*
    if (_visible) {
        _adaptSize(_width, _height);    
    }
    */
    return true;
}

void WindowWayland::_makeCursors() {
    
    if (theme)
        wl_cursor_theme_destroy(theme);
    theme = wl_cursor_theme_load(nullptr, 24 * _scale, _windowManager.shm);
}
// cursed
wl_cursor* WindowWayland::_getCursorFor(jwm::MouseCursor cursor) {
    switch (cursor) {
        case jwm::MouseCursor::ARROW:
            // works
            return wl_cursor_theme_get_cursor(theme, "default");
        case jwm::MouseCursor::CROSSHAIR:
            // works
            return wl_cursor_theme_get_cursor(theme, "crosshair");
        case jwm::MouseCursor::HELP:
            // sometimes works?
            return wl_cursor_theme_get_cursor(theme, "help");
        case jwm::MouseCursor::POINTING_HAND:
            // SHOULD work
            return wl_cursor_theme_get_cursor(theme, "pointer");
        case jwm::MouseCursor::IBEAM:
            // doesn't work at all
            return wl_cursor_theme_get_cursor(theme, "text");
        case jwm::MouseCursor::NOT_ALLOWED:
            return wl_cursor_theme_get_cursor(theme, "not-allowed");
        case jwm::MouseCursor::WAIT:
            return wl_cursor_theme_get_cursor(theme, "watch");
        case jwm::MouseCursor::WIN_UPARROW:
            return wl_cursor_theme_get_cursor(theme, "up-arrow");
        case jwm::MouseCursor::RESIZE_NS:
            return wl_cursor_theme_get_cursor(theme, "ns-resize");
        case jwm::MouseCursor::RESIZE_WE:
            return wl_cursor_theme_get_cursor(theme, "ew-resize");
        case jwm::MouseCursor::RESIZE_NESW:
            return wl_cursor_theme_get_cursor(theme, "nesw-resize");
        case jwm::MouseCursor::RESIZE_NWSE:
            return wl_cursor_theme_get_cursor(theme, "nwse-resize");
    }
    return nullptr;
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
    return getUnscaledWidth() * _scale;
}
int WindowWayland::getUnscaledWidth() {
    return _width <= 0 ? _floatingWidth : _width;
}
int WindowWayland::getHeight() {
    return getUnscaledHeight() * _scale;
}
int WindowWayland::getUnscaledHeight() {
    return _height <= 0 ? _floatingHeight : _height;
}

float WindowWayland::getScale() {
    return _scale;
}
bool WindowWayland::init() {
    _waylandWindow = wl_compositor_create_surface(_windowManager.compositor);
    wl_surface_add_listener(_waylandWindow, &_surfaceListener, this);
    // unsure if listener data and user data are the same, so i do this for safety : )
    wl_surface_set_user_data(_waylandWindow, this);
    wl_proxy_set_tag((wl_proxy*) _waylandWindow, &_windowTag);

    _windowManager.registerWindow(this);
    _configured = false;
    return true;
}
void WindowWayland::show()
{
    _frame = libdecor_decorate(_windowManager.decorCtx, _waylandWindow, &_libdecorFrameInterface, this);
    libdecor_frame_map(_frame);
    wl_display_roundtrip(_windowManager.display);
    setTitle(_title);
    setTitlebarVisible(_titlebarVisible);
    _visible = true;
}

ScreenInfo WindowWayland::getScreen() {
    if (_output) {
        return _output->getScreenInfo();
    } else {
        return {
            .id = -1,
            .bounds = jwm::IRect::makeXYWH(0, 0, _width, _height),
            .isPrimary = false,
            .scale = _scale
        };
    }
}
void WindowWayland::setVisible(bool isVisible) {
    if (_visible != isVisible) {
        _visible = isVisible;
        if (_visible) {
            show();
        } else {
            hide();
        }
    }
}

void jwm::WindowWayland::setCursor(jwm::MouseCursor cursor) {
    // ?????
    // Doesn't work for higher numbers???
    auto wayCursor = _getCursorFor(cursor)->images[0];
    auto buf = wl_cursor_image_get_buffer(wayCursor);
    wl_surface_attach(_windowManager.cursorSurface, 
            wl_cursor_image_get_buffer(wayCursor),
            0, 0);
    wl_surface_set_buffer_scale(_windowManager.cursorSurface, _scale);
    wl_surface_damage_buffer(_windowManager.cursorSurface, 0, 0, INT32_MAX, INT32_MAX);
    wl_pointer_set_cursor(_windowManager.pointer, _windowManager.mouseSerial, _windowManager.cursorSurface, 
            wayCursor->hotspot_x / _scale, wayCursor->hotspot_y / _scale);
    wl_surface_commit(_windowManager.cursorSurface);
}

// what do???
void jwm::WindowWayland::surfaceEnter(void* data, wl_surface* surface, wl_output* output) {
    // doesn't crash : )
    WindowWayland* self = reinterpret_cast<WindowWayland*>(data);
    
    for (auto o : self->_windowManager.outputs) {
        if (o->_output == output) {
            self->_output = o;
            self->_scale = o->scale;
            wl_surface_set_buffer_scale(surface, o->scale);
            self->_adaptSize(self->getUnscaledWidth(), self->getUnscaledHeight());
            break;
        }
    }
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
    wl_surface_set_buffer_scale(surface, factor);
    self->_adaptSize(self->getUnscaledWidth(), self->getUnscaledHeight());
}
void jwm::WindowWayland::surfacePreferredBufferTransform(void* data, wl_surface* surface, uint32_t transform) {}

void jwm::WindowWayland::outputGeometry(void* data, wl_output* output, int x, int y, int pWidth, int pHeight,
        int subpixel, const char* make, const char* model, int transform) {}
void jwm::WindowWayland::outputMode(void* data, wl_output* output, uint32_t flags, int width, int height,
        int refresh) {}
void jwm::WindowWayland::outputDone(void* data, wl_output* output) {}
void jwm::WindowWayland::outputScale(void* data, wl_output* output, int factor) {
    WindowWayland* self = reinterpret_cast<WindowWayland*>(data);
    self->_scale = factor;
    if (self->_waylandWindow)
        self->dispatch(classes::EventWindowScreenChange::kInstance);
}
void jwm::WindowWayland::outputName(void* data, wl_output* output, const char* name) {}
void jwm::WindowWayland::outputDescription(void* data, wl_output* output, const char* desc) {}
wl_callback_listener jwm::WindowWayland::_frameCallback = {
    .done = [](void* data, wl_callback* cb, uint32_t cb_data) {
        auto self = reinterpret_cast<WindowWayland*>(data);
        self->_adaptSize(self->_newWidth, self->_newHeight);
    }
};

void jwm::WindowWayland::decorFrameConfigure(libdecor_frame* frame, libdecor_configuration* configuration,
        void *userData) {
    auto self = reinterpret_cast<WindowWayland*>(userData);
    int width = 0, height = 0;
    libdecor_window_state winState;

    libdecor_configuration_get_content_size(configuration, frame, &width, &height);


    width = (width <= 0) ? self->_floatingWidth : width;
    height = (height <= 0) ? self->_floatingHeight : height;

    libdecor_state* state = libdecor_state_new(width, height);
    libdecor_frame_commit(frame, state, configuration);
    libdecor_state_free(state);

    if (libdecor_configuration_get_window_state(configuration, &winState)) {
        bool active = (winState & LIBDECOR_WINDOW_STATE_ACTIVE) != 0;
        bool maximized = (winState & LIBDECOR_WINDOW_STATE_MAXIMIZED) != 0;
        bool fullscreen = (winState & LIBDECOR_WINDOW_STATE_FULLSCREEN) != 0;
        // Some compositors (like weston) don't actually tell me on focus in and focus out.
        // Libdecor simply sends an active at the beginning and keeps chugging.
        if (active != self->_active)
            if (active)
                self->dispatch(classes::EventWindowFocusIn::kInstance);
            else
                self->dispatch(classes::EventWindowFocusOut::kInstance);
        self->_active = active;
        if (maximized != self->_maximized)
            if (maximized)
                self->dispatch(classes::EventWindowMaximize::kInstance);
        self->_maximized = maximized;
        // ???
        /*
        if (fullscreen != self->_fullscreen)
            if (fullscreen)
                self->dispatch(classes::EventWindowFullScreenEnter::kInstance);
            else
                self->dispatch(classes::EventWindowFullScreenLeave::kInstance);
                */
        self->_fullscreen = fullscreen;
        self->_floating = libdecor_frame_is_floating(frame);
    }
    if (self->_width != width || self->_height != height) {
        if (libdecor_frame_is_floating(frame)) {
            if (width > 0) 
                self->_floatingWidth = width;
            if (height > 0)
                self->_floatingHeight = height;
        }


        self->_newWidth = width;
        self->_newHeight = height;
        // This flat out breaks window if it isn't throttled
        wl_callback* callback = wl_surface_frame(self->_waylandWindow);
        // Throttle frame
        wl_callback_add_listener(callback, &_frameCallback, self);
    }
    if (!self->_configured && self->_visible) {
        if (self->_layer)
            self->_layer->attachBuffer();
    }
    self->_configured = true;
}
void jwm::WindowWayland::decorFrameClose(libdecor_frame* frame, void* userData) {
    WindowWayland* self = reinterpret_cast<WindowWayland*>(userData);
    self->dispatch(classes::EventWindowCloseRequest::kInstance);
}
void jwm::WindowWayland::decorFrameCommit(libdecor_frame* frame, void* userData) {
    WindowWayland* self = reinterpret_cast<WindowWayland*>(userData);
    if (self->_waylandWindow) {
        wl_surface_commit(self->_waylandWindow);
        wl_display_roundtrip(self->_windowManager.display);
    }
}
void jwm::WindowWayland::decorFrameDismissPopup(libdecor_frame* frame, const char* seatName, void* userData) {}
void jwm::WindowWayland::_adaptSize(int newWidth, int newHeight) {
    using namespace classes;
    _width = newWidth;
    _height = newHeight;
    int scaledWidth = _width * _scale;
    int scaledHeight = _height * _scale;
    jwm::JNILocal<jobject> eventWindowResize(
               app.getJniEnv(),
               EventWindowResize::make(
                        app.getJniEnv(),
                        scaledWidth,
                        scaledHeight,
                        scaledWidth,
                        scaledHeight
                   )
            );
    dispatch(eventWindowResize.get());
    if (_scale != _oldScale)
        _makeCursors();
    _oldScale = _scale;
    // In Java Wayland doesn't actually cause a frame:
    // however decorFrameCommit will cause a redraw anyway.
    // Not doing it in wayland lets me not cause an exception on hide.
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

extern "C" JNIEXPORT void JNICALL Java_io_github_humbleui_jwm_WindowWayland__1nSetContentSize
    (JNIEnv* env, jobject obj, jint width, jint height) {
    reinterpret_cast<jwm::WindowWayland*>(jwm::classes::Native::fromJava(env, obj))->resize(width, height);    
}
extern "C" JNIEXPORT jobject JNICALL Java_io_github_humbleui_jwm_WindowWayland__1nGetScreen
    (JNIEnv* env, jobject obj) {
    jwm::WindowWayland* instance = reinterpret_cast<jwm::WindowWayland*>(jwm::classes::Native::fromJava(env, obj));
    return instance->getScreen().asJavaObject(env);
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
        (JNIEnv* env, jobject obj, jstring title) {
    jwm::WindowWayland* instance = reinterpret_cast<jwm::WindowWayland*>(jwm::classes::Native::fromJava(env, obj));

    const jchar* bytes = env->GetStringChars(title, nullptr);
    jsize length = env->GetStringLength(title);
    std::u16string thingie = {reinterpret_cast<const char16_t*>(bytes), static_cast<size_t>(length)};

    std::string titleS = std::wstring_convert<
            std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(thingie);
    env->ReleaseStringChars(title, bytes);
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
