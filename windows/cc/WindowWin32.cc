#include <PlatformWin32.hh>
#include <AppWin32.hh>
#include <WindowWin32.hh>
#include <WindowManagerWin32.hh>
#include <Key.hh>
#include <KeyModifier.hh>
#include <MouseButton.hh>
#include <impl/Library.hh>
#include <impl/JNILocal.hh>
#include <memory>

jwm::WindowWin32::WindowWin32(JNIEnv *env, class WindowManagerWin32 &windowManagerWin32)
        : Window(env), _windowManager(windowManagerWin32) {

}

jwm::WindowWin32::~WindowWin32() {
    _close();
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
        AppWin32::getInstance().sendError("Failed to init WindowWin32");
        return false;
    }

    // Set this as property to reference from message callbacks
    SetPropW(_hWnd, L"JWM", this);

    // Register window, so manager can process its update
    _windowManager._registerWindow(*this);

    return true;
}

void jwm::WindowWin32::start() {
    bool destroyed = false;

    // Listen for window destroy event
    // if window is destroyed while processing some event,
    // we will know about that (leave event loop and start function)
    addEventListener([&](Event event){
        if (event == Event::Destroyed) {
            destroyed = true;
        }
    });

    AppWin32& app = AppWin32::getInstance();
    JNIEnv* env = getJNIEnv();

    while (!_shouldClose.load() && !app.isTerminateRequested()) {
        MSG msg;

        while (PeekMessageW(&msg, _hWnd, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);

            if (destroyed)
                return;
        }

        // If frame was requested, we must dispatch this event here.
        // Note about context switch (if window in the separate thread it is not required)
        if (testFlag(Flag::RequestFrame)) {
            removeFlag(Flag::RequestFrame);
            notifyEvent(Event::SwitchContext);
            dispatch(classes::EventFrame::kInstance);
            notifyEvent(Event::SwapBuffers);
        }

        // Process window thread callbacks
        std::vector<jobject> toProcess;
        {
            std::lock_guard<std::mutex> lock(_accessMutex);
            std::swap(toProcess, _callbacks);
        }

        for (auto callback: toProcess) {
            classes::Runnable::run(env, callback);
            env->DeleteGlobalRef(callback);
        }
    }

    printf("jwm::WindowWin32::start()\n");
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

void jwm::WindowWin32::enqueueCallback(jobject callback) {
    std::lock_guard<std::mutex> lock(_accessMutex);
    _callbacks.push_back(callback);
}

void jwm::WindowWin32::requestClose() {
    _shouldClose.store(true);
}

LRESULT jwm::WindowWin32::processEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    JNIEnv* env = getJNIEnv();

    switch (uMsg) {
        // HACK: Set timer to get JWM_WM_FRAME_TIMER event.
        // When user hold mouse button and drag window, app enter modal loop,
        // animation is stopped. This hack allows us to get JWM_WM_FRAME_TIMER
        // event with minimum possible delay to repaint window and animate it.

        case WM_ENTERSIZEMOVE:
            setFlag(Flag::EnterSizeMove);
            _setFrameTimer();
            return 0;

        case WM_EXITSIZEMOVE:
            removeFlag(Flag::EnterSizeMove);
            _killFrameTimer();
            return 0;

        case WM_SIZE: {
            int width = LOWORD(lParam);
            int height = HIWORD(lParam);
            JNILocal<jobject> eventResize(env, classes::EventResize::make(env, width, height));
            dispatch(eventResize.get());
            return 0;
        }

        case WM_MOVE: {
            int left = GET_X_LPARAM(lParam);
            int top = GET_Y_LPARAM(lParam);
            JNILocal<jobject> eventMove(env, classes::EventWindowMove::make(env, left, top));
            dispatch(eventMove.get());
            return 0;
        }

        case WM_ERASEBKGND:
            return true;

        case WM_TIMER: {
            if (wParam == JWM_WM_FRAME_TIMER) {
                // Repaint window if requested

                if (testFlag(Flag::RequestFrame)) {
                    removeFlag(Flag::RequestFrame);
                    dispatch(classes::EventFrame::kInstance);
                    notifyEvent(Event::SwapBuffers);
                }
            }

            return 0;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;

            if (BeginPaint(_hWnd, &ps)) {
                dispatch(classes::EventFrame::kInstance);
                notifyEvent(Event::SwapBuffers);
                EndPaint(_hWnd, &ps);
            }

            return 0;
        }

        case WM_MOUSEMOVE: {
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);
            int buttons = _getMouseButtons();
            int modifiers = _getModifiers();

            JNILocal<jobject> eventMouseMove(env, classes::EventMouseMove::make(env, xPos, yPos, buttons, modifiers));
            dispatch(eventMouseMove.get());
            return 0;
        }

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_XBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP: {
            bool isPressed =
                uMsg == WM_LBUTTONDOWN ||
                uMsg == WM_RBUTTONDOWN ||
                uMsg == WM_MBUTTONDOWN ||
                uMsg == WM_XBUTTONDOWN;

            int modifiers = _getModifiers();
            MouseButton button;

            if (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP)
                button = MouseButton::PRIMARY;
            else if (uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONUP)
                button = MouseButton::SECONDARY;
            else if (uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONUP)
                button = MouseButton::MIDDLE;
            else if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
                button = MouseButton::BACK;
            else
                button = MouseButton::FORWARD;

            JNILocal<jobject> eventMouseButton(env, classes::EventMouseButton::make(env, button, isPressed, modifiers));
            dispatch(eventMouseButton.get());
            break;
        }

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP: {
            bool isPressed = !(HIWORD(lParam) & KF_UP);
            int keycode = (int) wParam;
            int scancode = (int) MapVirtualKeyA((UINT) wParam, MAPVK_VK_TO_VSC);
            int modifiers = _getModifiers();
            auto& table = _windowManager.getKeyTable();
            auto mapping = table.find(keycode);

            Key key = mapping != table.end()? mapping->second: Key::UNDEFINED;

            // printf("WM_KEY win keycode: 0x%x win scancode: 0x%x Key: 0x%x isPressed:%i modifiers:%x\n",
            //       keycode, scancode, (int)key, (int)isPressed, modifiers);

            JNILocal<jobject> eventKeyboard(env, classes::EventKeyboard::make(env, key, isPressed, modifiers));
            dispatch(eventKeyboard.get());
            break;
        }

        case WM_CLOSE:
            dispatch(classes::EventClose::kInstance);
            return 0;

        default:
            break;
    }

    return DefWindowProcW(_hWnd, uMsg, wParam, lParam);
}

int jwm::WindowWin32::addEventListener(jwm::WindowWin32::Callback callback) {
    int callbackID = _getNextCallbackID();
    _eventListeners.emplace_back(callbackID, std::move(callback));
    return callbackID;
}

void jwm::WindowWin32::removeEventListener(int callbackID) {
    auto current = _eventListeners.begin();
    while (current != _eventListeners.end()) {
        if (current->first == callbackID)
            current = _eventListeners.erase(current);
        else
            ++current;
    }
}

void jwm::WindowWin32::notifyEvent(Event event) {
    for (auto& entry: _eventListeners)
        entry.second(event);
}

DWORD jwm::WindowWin32::_getWindowStyle() const {
    return WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
}

DWORD jwm::WindowWin32::_getWindowExStyle() const {
    return 0;
}

int jwm::WindowWin32::_getModifiers() const {
    int modifiers = 0;

    if (GetKeyState(VK_SHIFT) & BUTTON_DOWN)
        modifiers |= static_cast<int>(KeyModifier::SHIFT);

    if (GetKeyState(VK_CONTROL) & BUTTON_DOWN)
        modifiers |= static_cast<int>(KeyModifier::CONTROL);

    if (GetKeyState(VK_MENU) & BUTTON_DOWN)
        modifiers |= static_cast<int>(KeyModifier::ALT);

    if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & BUTTON_DOWN)
        modifiers |= static_cast<int>(KeyModifier::WINDOWS);

    return modifiers;
}

int jwm::WindowWin32::_getMouseButtons() const {
    int buttons = 0;

    if (GetKeyState(VK_LBUTTON) & BUTTON_DOWN)
        buttons |= static_cast<int>(MouseButton::PRIMARY);

    if (GetKeyState(VK_RBUTTON) & BUTTON_DOWN)
        buttons |= static_cast<int>(MouseButton::SECONDARY);

    if (GetKeyState(VK_MBUTTON) & BUTTON_DOWN)
        buttons |= static_cast<int>(MouseButton::MIDDLE);

    if (GetKeyState(VK_XBUTTON1) & BUTTON_DOWN)
        buttons |= static_cast<int>(MouseButton::BACK);

    if (GetKeyState(VK_XBUTTON2) & BUTTON_DOWN)
        buttons |= static_cast<int>(MouseButton::FORWARD);

    return buttons;
}

int jwm::WindowWin32::_getNextCallbackID() {
    return _nextCallbackID++;
}

void jwm::WindowWin32::_setFrameTimer() {
    SetTimer(_hWnd, JWM_WM_FRAME_TIMER, USER_TIMER_MINIMUM, nullptr);
}

void jwm::WindowWin32::_killFrameTimer() {
    KillTimer(_hWnd, JWM_WM_FRAME_TIMER);
}

void jwm::WindowWin32::_close() {
    if (_hWnd) {
        // Notify listeners that window is going to be destroyed
        notifyEvent(Event::Destroyed);
        _eventListeners.clear();

        JNIEnv* env = getJNIEnv();

        // Clear pending callbacks if present
        for (auto callback: _callbacks)
            env->DeleteGlobalRef(callback);
        _callbacks.clear();

        // Remove window from manager and destroy OS obejct
        _windowManager._unregisterWindow(*this);
        DestroyWindow(_hWnd);
        _hWnd = nullptr;
    }
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
    instance->setFlag(jwm::WindowWin32::Flag::RequestFrame);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowWin32__1nStart
        (JNIEnv* env, jobject obj) {
    jwm::WindowWin32* instance = reinterpret_cast<jwm::WindowWin32*>(jwm::classes::Native::fromJava(env, obj));
    instance->start();
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowWin32__1nRunOnWindowThread
        (JNIEnv* env, jobject obj, jobject callback) {
    jwm::WindowWin32* instance = reinterpret_cast<jwm::WindowWin32*>(jwm::classes::Native::fromJava(env, obj));
    auto callbackRef = env->NewGlobalRef(callback);
    instance->enqueueCallback(callbackRef);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowWin32__1nClose
        (JNIEnv* env, jobject obj) {
    jwm::WindowWin32* instance = reinterpret_cast<jwm::WindowWin32*>(jwm::classes::Native::fromJava(env, obj));
    instance->requestClose();
}