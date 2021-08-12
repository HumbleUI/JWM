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
    int x = CW_USEDEFAULT;
    int y = CW_USEDEFAULT;
    int width = CW_USEDEFAULT;
    int height = CW_USEDEFAULT;
    const wchar_t* caption = JWM_WIN32_WINDOW_DEFAULT_NAME;

    return _createInternal(x, y, width, height, caption);
}

void jwm::WindowWin32::recreate() {
    int x;
    int y;
    int width;
    int height;
    const wchar_t* caption = JWM_WIN32_WINDOW_DEFAULT_NAME;

    getPosition(x, y);
    getSize(width, height);

    setFlag(Flag::IgnoreMessages);

    _destroyInternal();
    _createInternal(x, y, width, height, caption);

    show();

    removeFlag(Flag::IgnoreMessages);
}

void jwm::WindowWin32::unmarkText() {
    _imeResetComposition();
}

void jwm::WindowWin32::setImeEnabled(bool enabled) {
    if (enabled)
        // Re-enabled Windows IME by associating default context.
        ImmAssociateContextEx(getHWnd(), nullptr, IACE_DEFAULT);
    else
        // Disable Windows IME by associating 0 context.
        ImmAssociateContext(getHWnd(), nullptr);
}

void jwm::WindowWin32::show() {
    ShowWindow(_hWnd, SW_SHOWNA);
}

void jwm::WindowWin32::getPosition(int &left, int &top) const {
    RECT area;
    GetWindowRect(_hWnd, &area);

    left = area.left;
    top = area.top;
}

void jwm::WindowWin32::getSize(int &width, int &height) const {
    RECT area;
    GetWindowRect(_hWnd, &area);

    width = area.right - area.left;
    height = area.bottom - area.top;

    // Explicitly clamp size, since w or h cannot be less than 0
    width = width > 1? width: 1;
    height = height > 1? height: 1;
}

void jwm::WindowWin32::getClientAreaSize(int &width, int &height) const {
    RECT area;
    GetClientRect(_hWnd, &area);

    width = area.right - area.left;
    height = area.bottom - area.top;

    // Explicitly clamp size, since w or h cannot be less than 0
    width = width > 1? width: 1;
    height = height > 1? height: 1;
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
    getClientAreaSize(width, height);
    return width;
}

int jwm::WindowWin32::getHeight() const {
    int width, height;
    getClientAreaSize(width, height);
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

    SetWindowPos(_hWnd, nullptr,
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
    _close();
}

LRESULT jwm::WindowWin32::processEvent(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (testFlag(Flag::IgnoreMessages))
        return DefWindowProcW(_hWnd, uMsg, wParam, lParam);

    JNIEnv* env = getJNIEnv();

    notifyEvent(Event::SwitchContext);

    switch (uMsg) {
        // HACK: Set timer to get JWM_WM_TIMER_UPDATE_EVENT event.
        // When user hold mouse button and drag window, app enter modal loop,
        // animation is stopped. This hack allows us to get JWM_WM_TIMER_UPDATE_EVENT
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
            width = width > 1? width: 1;
            height = height > 1? height: 1;
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
            if (wParam == JWM_WM_TIMER_UPDATE_EVENT) {
                // HACK: modal event loop on move/resize for focus window blocks background windows.
                // Therefore they are freeze. Here we can receive update event inside modal loop
                // to get chance to process frame event and etc.
                _windowManager.timerUpdate();
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

        case WM_MOUSEWHEEL: {
            auto nativeScroll = static_cast<SHORT>(HIWORD(wParam));
            auto scrollScale = static_cast<float>(_getWheelScrollLines());
            auto scrollValue = scrollScale * static_cast<float>(nativeScroll) / static_cast<float>(WHEEL_DELTA);
            int modifiers = _getModifiers();

            // NOTE: scroll direction may differ from macOS or Linux
            JNILocal<jobject> eventScroll(env, classes::EventScroll::make(env, 0.0f, scrollValue, modifiers));
            dispatch(eventScroll.get());
            return 0;
        }

        case WM_MOUSEHWHEEL: {
            auto nativeScroll = static_cast<SHORT>(HIWORD(wParam));
            auto scrollScale = static_cast<float>(_getWheelScrollLines());
            auto scrollValue = scrollScale * static_cast<float>(nativeScroll) / static_cast<float>(WHEEL_DELTA);
            int modifiers = _getModifiers();

            // NOTE: scroll direction may differ from macOS or Linux
            JNILocal<jobject> eventScroll(env, classes::EventScroll::make(env, -scrollValue, 0.0f, modifiers));
            dispatch(eventScroll.get());
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
            int modifiers = _getModifiers();
            auto& table = _windowManager.getKeyTable();
            auto mapping = table.find(keycode);

            Key key = mapping != table.end()? mapping->second: Key::UNDEFINED;

            JNILocal<jobject> eventKeyboard(env, classes::EventKeyboard::make(env, key, isPressed, modifiers));
            dispatch(eventKeyboard.get());
            break;
        }

        case WM_CHAR:
        case WM_SYSCHAR: {
            if (HIGH_SURROGATE_L <= wParam && wParam <= HIGH_SURROGATE_U) {
                _highSurrogate = static_cast<wchar_t>(wParam);
            }
            else {
                jsize len = 0;
                jchar text[2];

                if (LOW_SURROGATE_L <= wParam && wParam <= LOW_SURROGATE_U) {
                    if (_highSurrogate) {
                        text[len] = _highSurrogate;
                        len += 1;
                    }
                }
                else if (wParam == VK_BACK) {
                    break;
                }

                text[len] = static_cast<wchar_t>(wParam);
                len += 1;

                _highSurrogate = 0;

                JNILocal<jstring> jtext(env, env->NewString(text, len));
                JNILocal<jobject> eventTextInput(env, classes::EventTextInput::make(env, jtext.get()));
                dispatch(eventTextInput.get());
            }

            if (uMsg == WM_SYSCHAR)
                break;

            return 0;
        }

        case WM_IME_SETCONTEXT:
            printf("WM_IME_SETCONTEXT\n");
            fflush(stdout);
            break;
        case WM_IME_STARTCOMPOSITION:
            printf("WM_IME_STARTCOMPOSITION\n");
            fflush(stdout);
            _compositionStr.resize(0);
            _compositionPos = 0;
            _imeChangeCursorPos();
            return true;

        case WM_IME_CHAR:
            return 0;

        case WM_IME_COMPOSITION: {
            int iParam = (int)lParam;

            HIMC hImc = ImmGetContext(getHWnd());

            if (!hImc)
                return false;

            if (iParam & (GCS_COMPSTR | GCS_COMPATTR | GCS_CURSORPOS)) {
                _compositionStr = std::move(_imeGetCompositionString(hImc, GCS_COMPSTR));
                _compositionPos = ImmGetCompositionStringW(hImc, GCS_CURSORPOS, nullptr, 0);

                int selectedFrom, selectedTo;
                _imeGetCompositionStringConvertedRange(hImc, selectedFrom, selectedTo);

                if ((lParam & CS_INSERTCHAR) && (lParam & CS_NOMOVECARET)) {
                    selectedFrom = 0;
                    selectedTo = static_cast<int>(_compositionStr.length());
                }

                if (!selectedTo)
                    selectedFrom = 0;

                auto jcharText = reinterpret_cast<const jchar*>(_compositionStr.c_str());
                auto jlength = static_cast<jsize>(_compositionStr.length());

                JNILocal<jstring> text(env, env->NewString(jcharText, jlength));
                JNILocal<jobject> eventTextInputMarked(env, classes::EventTextInputMarked::make(env, text.get(), selectedFrom, selectedTo));
                dispatch(eventTextInputMarked.get());
            }

            if (iParam & (GCS_RESULTSTR)) {
                _compositionStr = std::move(_imeGetCompositionString(hImc, GCS_RESULTSTR));

                auto jcharText = reinterpret_cast<const jchar*>(_compositionStr.c_str());
                auto jlength = static_cast<jsize>(_compositionStr.length());

                JNILocal<jstring> text(env, env->NewString(jcharText, jlength));
                JNILocal<jobject> eventTextInput(env, classes::EventTextInput::make(env, text.get()));
                dispatch(eventTextInput.get());
            }

            ImmReleaseContext(getHWnd(), hImc);
            return true;
        }

        case WM_IME_ENDCOMPOSITION:
            return true;

        case WM_IME_REQUEST:
            if (wParam == IMR_QUERYCHARPOSITION) {
                jobject uiRect = classes::TextInputClient::rectForMarkedRange(getJNIEnv(), this, 0, 0);

                if (!uiRect)
                    break;

                int left, top, right, bottom;
                classes::UIRect::getRectFields(getJNIEnv(), uiRect, left, top, right, bottom);

                POINT coords;
                coords.x = left;
                coords.y = top;
                ClientToScreen(getHWnd(), &coords);

                auto* imeCharPos = reinterpret_cast<IMECHARPOSITION*>(lParam);
                imeCharPos->dwCharPos = 0;
                imeCharPos->pt.x = coords.x;
                imeCharPos->pt.y = coords.y;

                printf("imeCharPos->pt.x %i imeCharPos->pt.y %i\n", imeCharPos->pt.x, imeCharPos->pt.y);
                return true;
            }
            break;

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

UINT jwm::WindowWin32::_getWheelScrollLines() const {
    UINT numLines;
    SystemParametersInfoW(SPI_GETWHEELSCROLLLINES, 0, &numLines, 0);
    return numLines;
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
    SetTimer(_hWnd, JWM_WM_TIMER_UPDATE_EVENT, USER_TIMER_MINIMUM, nullptr);
}

void jwm::WindowWin32::_killFrameTimer() {
    KillTimer(_hWnd, JWM_WM_TIMER_UPDATE_EVENT);
}

bool jwm::WindowWin32::_createInternal(int x, int y, int w, int h, const wchar_t *caption) {
    DWORD style = _getWindowStyle();
    DWORD exStyle = _getWindowExStyle();

    HWND hWndParent = nullptr;
    HMENU hMenu = nullptr;
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    LPVOID lpParam = nullptr;

    _hWnd = CreateWindowExW(
        exStyle,
        JWM_WIN32_WINDOW_CLASS_NAME,
        caption,
        style,
        x, y,
        w, h,
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

void jwm::WindowWin32::_destroyInternal() {
    // Remove window from manager and destroy OS object
    _windowManager._unregisterWindow(*this);
    DestroyWindow(_hWnd);
    _hWnd = nullptr;
}

void jwm::WindowWin32::_close() {
    if (_hWnd) {
        // Notify listeners that window is going to be destroyed
        notifyEvent(Event::Destroyed);
        _eventListeners.clear();

        // Native clean-up
        _destroyInternal();
    }
}

void jwm::WindowWin32::_imeResetComposition() const {
    HIMC hImc = ImmGetContext(getHWnd());

    if (!hImc)
        return;

    ImmNotifyIME(hImc, NI_COMPOSITIONSTR, CPS_CANCEL, 0);
    ImmReleaseContext(getHWnd(), hImc);
}

void jwm::WindowWin32::_imeChangeCursorPos() const {
    jobject uiRect = classes::TextInputClient::rectForMarkedRange(getJNIEnv(), this, 0, 0);

    if (!uiRect)
        return;

    int left, top, right, bottom;
    classes::UIRect::getRectFields(getJNIEnv(), uiRect, left, top, right, bottom);

    HIMC hImc = ImmGetContext(getHWnd());

    if (!hImc)
        return;

    COMPOSITIONFORM compositionform;
    compositionform.dwStyle = CFS_FORCE_POSITION;
    compositionform.ptCurrentPos.x = left;
    compositionform.ptCurrentPos.y = bottom;

    CANDIDATEFORM candidateform;
    candidateform.dwIndex = 0;
    candidateform.dwStyle = CFS_EXCLUDE;
    candidateform.ptCurrentPos.x = left;
    candidateform.ptCurrentPos.y = top;
    candidateform.rcArea.left = left;
    candidateform.rcArea.top = top;
    candidateform.rcArea.right = right;
    candidateform.rcArea.bottom = bottom;

    ImmSetCompositionWindow(hImc, &compositionform);
    ImmSetCandidateWindow(hImc, &candidateform);

    ImmReleaseContext(getHWnd(), hImc);
}

void jwm::WindowWin32::_imeGetCompositionStringConvertedRange(HIMC hImc, int &selFrom, int &selTo) const {
    selFrom = selTo = 0;

    // Size in bytes without terminating null character
    DWORD sizeBytes = ImmGetCompositionStringW(hImc, GCS_COMPATTR, nullptr, 0);

    if (sizeBytes) {
        std::unique_ptr<uint8_t[]> buffer{new uint8_t[sizeBytes]};
        ImmGetCompositionStringW(hImc, GCS_COMPATTR, buffer.get(), sizeBytes);

        int start = 0;

        while (start < static_cast<int>(sizeBytes) && !(buffer[start] & ATTR_TARGET_CONVERTED))
            start += 1;

        if (start < static_cast<int>(sizeBytes)) {
            int end = start + 1;

            while (end < static_cast<int>(sizeBytes) && (buffer[end] & ATTR_TARGET_CONVERTED))
                end += 1;

            selFrom = start;
            selTo = end;
        }
    }
}

std::wstring jwm::WindowWin32::_imeGetCompositionString(HIMC hImc, DWORD compType) const {
    // Size in bytes without terminating null character
    DWORD sizeBytes = ImmGetCompositionStringW(hImc, compType, nullptr, 0);

    if (sizeBytes) {
        // Get actual string and copy into tmp buffer
        std::unique_ptr<uint8_t[]> buffer{new uint8_t[sizeBytes]};
        ImmGetCompositionStringW(hImc, compType, buffer.get(), sizeBytes);
        return std::wstring(reinterpret_cast<wchar_t*>(buffer.get()), sizeBytes / sizeof(wchar_t));
    }

    return std::wstring();
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

extern "C" JNIEXPORT jobject JNICALL Java_org_jetbrains_jwm_WindowWin32_setTextInputEnabled
        (JNIEnv* env, jobject obj, jboolean enabled) {
    jwm::WindowWin32* instance = reinterpret_cast<jwm::WindowWin32*>(jwm::classes::Native::fromJava(env, obj));
    instance->setImeEnabled(enabled);
    return obj;
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowWin32_unmarkText
        (JNIEnv* env, jobject obj) {
    jwm::WindowWin32* instance = reinterpret_cast<jwm::WindowWin32*>(jwm::classes::Native::fromJava(env, obj));
    instance->unmarkText();
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

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowWin32__1nRunOnWindowThread
        (JNIEnv* env, jobject obj, jobject callback) {
    jwm::AppWin32& app = jwm::AppWin32::getInstance();
    jobject callbackRef = env->NewGlobalRef(callback);
    app.enqueueCallback(callbackRef);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowWin32__1nClose
        (JNIEnv* env, jobject obj) {
    jwm::WindowWin32* instance = reinterpret_cast<jwm::WindowWin32*>(jwm::classes::Native::fromJava(env, obj));
    instance->close();
}