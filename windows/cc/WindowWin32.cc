#include <PlatformWin32.hh>
#include <AppWin32.hh>
#include <WindowWin32.hh>
#include <WindowManagerWin32.hh>
#include <impl/Library.hh>
#include <memory>

jwm::WindowWin32::WindowWin32(JNIEnv *env, class WindowManagerWin32 &windowManagerWin32)
        : Window(env), _windowManager(windowManagerWin32) {

}

jwm::WindowWin32::~WindowWin32() {
    close();
}

bool jwm::WindowWin32::init() {
    DWORD style = _getWindowStyle();
    DWORD exStyle = _getWindowExStyle();

    int x = CW_USEDEFAULT;
    int y = CW_USEDEFAULT;
    int width = CW_USEDEFAULT;
    int height = CW_USEDEFAULT;

    HWND hWndParent = NULL;
    HMENU hMenu = NULL;
    HINSTANCE hInstance = GetModuleHandle(NULL);
    LPVOID lpParam = NULL;

    _hWnd = CreateWindowExW(
            exStyle,
            JWM_WIN32_WINDOW_CLASS_NAME,
            JWM_WIN32_WINDOW_DEFAULT_NAME,
            style,
            x, y,
            width, height,
            hWndParent,
            hMenu,
            hInstance,
            lpParam
    );

    if (!_hWnd) {
        _windowManager.sendError("Failed to init WindowWin32");
        return JWM_FALSE;
    }

    // Set this as property to reference from message callbacks
    SetPropW(_hWnd, L"JWM", this);

    // Register window, so manager can process its update
    _windowManager._registerWindow(*this);

    return JWM_TRUE;
}

void jwm::WindowWin32::show() {
    ShowWindow(_hWnd, SW_SHOWNA);
}

void jwm::WindowWin32::getPosition(int &left, int &top) const {
    POINT pos = { 0, 0 };
    ClientToScreen(_hWnd, &pos);

    left = pos.x;
    top = pos.y;
}

void jwm::WindowWin32::getSize(int &width, int &height) const {
    RECT area;
    GetClientRect(_hWnd, &area);

    // Explicitly clamp size, since w or h cannot be less than 0
    width = area.right > 0? area.right: 0;
    height = area.bottom > 0? area.bottom: 0;
}

int jwm::WindowWin32::getLeft() const {
    int left, top;
    getPosition(left, top);
    return left;
}

int jwm::WindowWin32::getTop() const {
    int left, top;
    getPosition(left, top);
    return top;
}

int jwm::WindowWin32::getWidth() const {
    int width, height;
    getSize(width, height);
    return width;
}

int jwm::WindowWin32::getHeight() const {
    int width, height;
    getSize(width, height);
    return height;
}

float jwm::WindowWin32::getScale() const {
    HMONITOR hMonitor = MonitorFromWindow(_hWnd, MONITOR_DEFAULTTOPRIMARY);
    DEVICE_SCALE_FACTOR scaleFactor;
    GetScaleFactorForMonitor(hMonitor, &scaleFactor);

    if (scaleFactor == DEVICE_SCALE_FACTOR_INVALID)
        scaleFactor = JWM_DEFAULT_DEVICE_SCALE;

    return (float) scaleFactor / (float) SCALE_100_PERCENT;
}

void jwm::WindowWin32::move(int left, int top) {
    RECT rect = { left, top, left, top };

    AdjustWindowRectEx(&rect, _getWindowStyle(),
                       FALSE, _getWindowExStyle());

    SetWindowPos(_hWnd, NULL,
                 rect.left, rect.top,
                 0, 0,
                 SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
}

void jwm::WindowWin32::resize(int width, int height) {
    RECT rect = { 0, 0, width, height };

    AdjustWindowRectEx(&rect, _getWindowStyle(),
                       FALSE, _getWindowExStyle());

    SetWindowPos(_hWnd, HWND_TOP,
                 0, 0,
                 rect.right - rect.left,
                 rect.bottom - rect.top,
                 SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOZORDER);
}

void jwm::WindowWin32::close() {
    if (_hWnd) {
        _windowManager._unregisterWindow(*this);
        DestroyWindow(_hWnd);
        _hWnd = NULL;
    }
}

DWORD jwm::WindowWin32::_getWindowStyle() const {
    return WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
}

DWORD jwm::WindowWin32::_getWindowExStyle() const {
    return 0;
}

// JNI

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_WindowWin32__1nMake
        (JNIEnv* env, jclass jclass) {
    std::unique_ptr<jwm::WindowWin32> instance(new jwm::WindowWin32(env, jwm::AppWin32::getInstance().getWindowManager()));
    if (instance->init())
        return reinterpret_cast<jlong>(instance.release());
    else
        return 0;
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowWin32_show
        (JNIEnv* env, jobject obj) {
    jwm::WindowWin32* instance = reinterpret_cast<jwm::WindowWin32*>(jwm::classes::Native::fromJava(env, obj));
    instance->show();
}

extern "C" JNIEXPORT jint JNICALL Java_org_jetbrains_jwm_WindowWin32_getLeft
        (JNIEnv* env, jobject obj) {
    jwm::WindowWin32* instance = reinterpret_cast<jwm::WindowWin32*>(jwm::classes::Native::fromJava(env, obj));
    return instance->getLeft();
}

extern "C" JNIEXPORT jint JNICALL Java_org_jetbrains_jwm_WindowWin32_getTop
        (JNIEnv* env, jobject obj) {
    jwm::WindowWin32* instance = reinterpret_cast<jwm::WindowWin32*>(jwm::classes::Native::fromJava(env, obj));
    return instance->getTop();
}

extern "C" JNIEXPORT jint JNICALL Java_org_jetbrains_jwm_WindowWin32_getWidth
        (JNIEnv* env, jobject obj) {
    jwm::WindowWin32* instance = reinterpret_cast<jwm::WindowWin32*>(jwm::classes::Native::fromJava(env, obj));
    return instance->getWidth();
}

extern "C" JNIEXPORT jint JNICALL Java_org_jetbrains_jwm_WindowWin32_getHeight
        (JNIEnv* env, jobject obj) {
    jwm::WindowWin32* instance = reinterpret_cast<jwm::WindowWin32*>(jwm::classes::Native::fromJava(env, obj));
    return instance->getHeight();
}

extern "C" JNIEXPORT jfloat JNICALL Java_org_jetbrains_jwm_WindowWin32_getScale
        (JNIEnv* env, jobject obj) {
    jwm::WindowWin32* instance = reinterpret_cast<jwm::WindowWin32*>(jwm::classes::Native::fromJava(env, obj));
    return instance->getScale();
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowWin32_move
        (JNIEnv* env, jobject obj, int left, int top) {
    jwm::WindowWin32* instance = reinterpret_cast<jwm::WindowWin32*>(jwm::classes::Native::fromJava(env, obj));
    instance->move(left, top);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowWin32_resize
        (JNIEnv* env, jobject obj, int width, int height) {
    jwm::WindowWin32* instance = reinterpret_cast<jwm::WindowWin32*>(jwm::classes::Native::fromJava(env, obj));
    instance->resize(width, height);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowWin32_requestFrame
        (JNIEnv* env, jobject obj) {
    jwm::WindowWin32* instance = reinterpret_cast<jwm::WindowWin32*>(jwm::classes::Native::fromJava(env, obj));
    jwm::AppWin32& app = jwm::AppWin32::getInstance();
    jwm::WindowManagerWin32& winMan = app.getWindowManager();
    winMan.requestFrame(instance);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowWin32__1nClose
        (JNIEnv* env, jobject obj) {
    jwm::WindowWin32* instance = reinterpret_cast<jwm::WindowWin32*>(jwm::classes::Native::fromJava(env, obj));
    instance->close();
}