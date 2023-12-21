#include "WindowWayland.hh"
#include <jni.h>
#include <memory>
#include <cstring>
#include "AppWayland.hh"
#include "impl/Library.hh"
#include "impl/JNILocal.hh"
#include <locale>
#include <codecvt>
#include <algorithm>

using namespace jwm;


wl_surface_listener WindowWayland::_surfaceListener = {
    .enter = WindowWayland::surfaceEnter,
    .leave = WindowWayland::surfaceLeave,
#ifdef HAVE_WAYLAND_1_22
    .preferred_buffer_scale = WindowWayland::surfacePreferredBufferScale,
    .preferred_buffer_transform = WindowWayland::surfacePreferredBufferTransform
#endif
};
 
libdecor_frame_interface WindowWayland::_libdecorFrameInterface = {
    .configure = WindowWayland::decorFrameConfigure,
    .close = WindowWayland::decorFrameClose,
    .commit = WindowWayland::decorFrameCommit,
    .dismiss_popup = WindowWayland::decorFrameDismissPopup
};

WindowWayland::WindowWayland(JNIEnv* env, WindowManagerWayland& windowManager):
    jwm::Window(env),
    _windowManager(windowManager),
    _title("")

{
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
    _closed = true;
    hide();
    if (_waylandWindow) {
        wl_surface_destroy(_waylandWindow);
    }
    _waylandWindow = nullptr;
    _windowManager.unregisterWindow(this);
    // TODO: more destruction!
}
void WindowWayland::hide() {
    _visible = false;
    if (_layer) {
        _layer->detachBuffer();
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
    _oldScale = _scale;
    return true;
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
wl_cursor* WindowWayland::_getCursorFor(jwm::MouseCursor cursor) {
    if (auto ptr = _windowManager.getPointer())
        return ptr->getCursorFor(_scale, cursor);
    return nullptr;
}
bool WindowWayland::init() {
    _waylandWindow = wl_compositor_create_surface(_windowManager.compositor);
    wl_surface_add_listener(_waylandWindow, &_surfaceListener, this);
    // unsure if listener data and user data are the same, so i do this for safety : )
    wl_surface_set_user_data(_waylandWindow, this);
    wl_proxy_set_tag((wl_proxy*) _waylandWindow, &AppWayland::proxyTag);

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
    if (!_outputs.empty()) {
        return _outputs.front()->getScreenInfo();
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
    if (!_windowManager.getPointer()) return;
    auto wayCursor = _getCursorFor(cursor)->images[0];
    auto buf = wl_cursor_image_get_buffer(wayCursor);
    auto cursorSurface = _windowManager.getCursorSurface();
    wl_surface_attach(cursorSurface, 
            wl_cursor_image_get_buffer(wayCursor),
            0, 0);
    wl_surface_set_buffer_scale(cursorSurface, _scale);
    wl_surface_damage_buffer(cursorSurface, 0, 0, INT32_MAX, INT32_MAX);
    _windowManager.getPointer()->updateHotspot(wayCursor->hotspot_x, wayCursor->hotspot_y);
    wl_surface_commit(cursorSurface);
}

// what do???
void jwm::WindowWayland::surfaceEnter(void* data, wl_surface* surface, wl_output* output) {
    // doesn't crash : )
    WindowWayland* self = reinterpret_cast<WindowWayland*>(data);
    
    if (auto out = Output::getForNative(output)) {
        self->_outputs.push_back(out);
        int scale = 1;
        for (auto i : self->_outputs) {
            if (i->scale > scale)
                scale = i->scale;
        }
        self->_scale = scale;
        self->_adaptSize(self->getUnscaledWidth(), self->getUnscaledHeight());
    }
}
bool jwm::WindowWayland::isNativeSelf(wl_surface* surface) {
    if (!_waylandWindow) return false;
    return surface == _waylandWindow;
}
bool jwm::WindowWayland::ownSurface(wl_surface* surface) {
    return AppWayland::ownProxy((wl_proxy*) surface);
}
void jwm::WindowWayland::surfaceLeave(void* data, wl_surface* surface, wl_output* output) {
    auto self = reinterpret_cast<WindowWayland*>(data);
    
    if (auto out = Output::getForNative(output)) {
        auto it = std::find(self->_outputs.begin(), self->_outputs.end(), out);

        if (it != self->_outputs.end())
            self->_outputs.erase(it);

        int scale = 1;
        for (auto i : self->_outputs) {
            if (i->scale > scale)
                scale = i->scale;
        }
        self->_scale = scale;
        self->_adaptSize(self->getUnscaledWidth(), self->getUnscaledHeight());
    }
}
void jwm::WindowWayland::surfacePreferredBufferScale(void* data, wl_surface* surface, int factor) {
    WindowWayland* self = (WindowWayland*) data;
    if (factor < 1) {
        return;
    }
    self->_scale = factor;
    // do I pinky promise here?
    // yes : )
    self->_adaptSize(self->getUnscaledWidth(), self->getUnscaledHeight());
}
void jwm::WindowWayland::surfacePreferredBufferTransform(void* data, wl_surface* surface, uint32_t transform) {}

static void frameCallbackDone(void* data, wl_callback* cb, uint32_t cb_data) {
    auto self = reinterpret_cast<WindowWayland*>(data);
    self->_adaptSize(self->_newWidth, self->_newHeight);
}
wl_callback_listener jwm::WindowWayland::_frameCallback = {
    .done = frameCallbackDone
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
    // before width
    if (!self->_configured) {
        if (self->_layer)
            self->_layer->attachBuffer();
    }
    if (self->_width != width || self->_height != height) {
        if (libdecor_frame_is_floating(frame)) {
            if (width > 0) 
                self->_floatingWidth = width;
            if (height > 0)
                self->_floatingHeight = height;
        }


        self->_adaptSize(width, height);
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
    }
}
void jwm::WindowWayland::decorFrameDismissPopup(libdecor_frame* frame, const char* seatName, void* userData) {}
void jwm::WindowWayland::_adaptSize(int newWidth, int newHeight) {
    using namespace classes;
    if (newWidth == _width && newHeight == _height && _scale == _oldScale) return;
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
