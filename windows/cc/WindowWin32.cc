#include <PlatformWin32.hh>
#include <AppWin32.hh>
#include <WindowWin32.hh>
#include <WindowManagerWin32.hh>
#include <Key.hh>
#include <KeyModifier.hh>
#include <MouseButton.hh>
#include <Log.hh>
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
    const wchar_t* caption = JWM_WIN32_WINDOW_DEFAULT_NAME;

    UIRect rect = getWindowRect();
    int x = rect.fLeft;
    int y = rect.fTop;
    int width = rect.getWidth();
    int height = rect.getHeight();

    setFlag(Flag::IgnoreMessages);

    _destroyInternal();
    _createInternal(x, y, width, height, caption);

    show();
    removeFlag(Flag::IgnoreMessages);
}

void jwm::WindowWin32::unmarkText() {
    JWM_VERBOSE("Request abort IME composition (unmarkText)");
    _imeResetComposition();
}

void jwm::WindowWin32::setImeEnabled(bool enabled) {
    if (enabled) {
        // Re-enabled Windows IME by associating default context.
        ImmAssociateContextEx(getHWnd(), nullptr, IACE_DEFAULT);
        JWM_VERBOSE("Enable ime text input")
    }
    else {
        // Disable Windows IME by associating 0 context.
        ImmAssociateContext(getHWnd(), nullptr);
        JWM_VERBOSE("Disable ime text input");
    }
}

void jwm::WindowWin32::setTitle(const std::wstring& title) {
    JWM_VERBOSE("Set window title '" << title << "'");
    SetWindowTextW(_hWnd, title.c_str());
}

void jwm::WindowWin32::setMouseCursor(MouseCursor cursor) {
    JWM_VERBOSE("Set window cursor '" << mouseCursorToStr(cursor) << "'");

    const wchar_t* cursorName = IDC_ARROW;

    switch (cursor) {
        case MouseCursor::ARROW:
            cursorName = IDC_ARROW;
            break;
        case MouseCursor::CROSSHAIR:
            cursorName = IDC_CROSS;
            break;
        case MouseCursor::HELP:
            cursorName = IDC_HELP;
            break;
        case MouseCursor::POINTING_HAND:
            cursorName = IDC_HAND;
            break;
        case MouseCursor::IBEAM:
            cursorName = IDC_IBEAM;
            break;
        case MouseCursor::UPARROW:
            cursorName = IDC_UPARROW;
            break;
        case MouseCursor::NOT_ALLOWED:
            cursorName = IDC_NO;
            break;
        case MouseCursor::WAIT:
            cursorName = IDC_WAIT;
            break;
        default:
            break;
    }

    if (_hMouseCursor) {
        DestroyCursor(_hMouseCursor);
        _hMouseCursor = nullptr;
    }

    _hMouseCursor = LoadCursorW(nullptr, cursorName);
    _setMouseCursorInternal();
}

void jwm::WindowWin32::show() {
    ShowWindow(_hWnd, SW_SHOWNA);
}

void jwm::WindowWin32::requestSwap() {
    if (testFlag(Flag::HasAttachedLayer)) {
        setFlag(Flag::RequestSwap);
    }
}

void jwm::WindowWin32::requestFrame() {
    if (testFlag(Flag::HasAttachedLayer)) {
        setFlag(Flag::RequestFrame);
        _windowManager.requestFrameEvent();
    }
}

jwm::UIRect jwm::WindowWin32::getWindowRect() const {
    RECT rect;
    GetWindowRect(_hWnd, &rect);
    return UIRect{rect.left, rect.top, rect.right, rect.bottom};
}

jwm::UIRect jwm::WindowWin32::getContentRect() const {
    RECT clientRect;
    GetClientRect(_hWnd, &clientRect);
    RECT windowRect;
    GetWindowRect(_hWnd, &windowRect);

    // Convert client area rect to screen space and
    POINT corners[] = {POINT{clientRect.left, clientRect.top}, POINT{clientRect.right, clientRect.bottom}};
    MapWindowPoints(getHWnd(), nullptr, corners, sizeof(corners)/sizeof(corners[0]));

    POINT leftTop = corners[0];
    POINT rightBottom = corners[1];

    // Build rect with client area rect relative to the window rect to handle title bar
    return UIRect{leftTop.x - windowRect.left, leftTop.y - windowRect.top,
                  rightBottom.x - windowRect.left, rightBottom.y - windowRect.top};
}

void jwm::WindowWin32::setWindowPosition(int left, int top) {
    JWM_VERBOSE("Set window position left=" << left << " top=" << top);

    SetWindowPos(_hWnd, nullptr,
                 left, top,
                 0, 0,
                 SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
}

void jwm::WindowWin32::setWindowSize(int width, int height) {
    JWM_VERBOSE("Set window size w=" << width << " h=" << height);

    SetWindowPos(_hWnd, HWND_TOP,
                 0, 0,
                 width,
                 height,
                 SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOZORDER);
}

void jwm::WindowWin32::setContentSize(int width, int height) {
    RECT rect{0, 0, width, height};

    AdjustWindowRectEx(&rect, _getWindowStyle(),
                       FALSE, _getWindowExStyle());

    JWM_VERBOSE("Set content size " << "w=" << width << " h=" << height << " "
                << "(adjusted window size" << " w=" << rect.right - rect.left << " h=" << rect.bottom - rect.top << ")");

    SetWindowPos(_hWnd, HWND_TOP,
                 0, 0,
                 rect.right - rect.left,
                 rect.bottom - rect.top,
                 SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOZORDER);
}

jwm::ScreenWin32 jwm::WindowWin32::getScreen() const {
    HMONITOR hMonitor = MonitorFromWindow(_hWnd, MONITOR_DEFAULTTOPRIMARY);
    return ScreenWin32::fromHMonitor(hMonitor);
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
            UIRect rect = getWindowRect();
            int windowWidth = rect.getWidth();
            int windowHeight = rect.getHeight();
            int contentWidth = LOWORD(lParam);
            int contentHeight = HIWORD(lParam);

            JWM_VERBOSE("Size event "
                        << "window w=" << windowWidth << " h=" << windowHeight << " "
                        << "content w=" << contentWidth << " h=" << contentHeight);

            JNILocal<jobject> eventWindowResize(env, classes::EventWindowResize::make(env, windowWidth, windowHeight,
                                                                                      contentWidth, contentHeight));
            dispatch(eventWindowResize.get());
            return 0;
        }

        case WM_MOVE: {
            UIRect rect = getWindowRect();
            int windowLeft= rect.fLeft;
            int windowTop = rect.fTop;

            JWM_VERBOSE("Move event left=" << windowLeft << " top" << windowTop)

            JNILocal<jobject> eventMove(env, classes::EventWindowMove::make(env, windowLeft, windowTop));
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
            JNILocal<jobject> eventMouseScroll(env, classes::EventMouseScroll::make(env, 0.0f, scrollValue, modifiers));
            dispatch(eventMouseScroll.get());
            return 0;
        }

        case WM_MOUSEHWHEEL: {
            auto nativeScroll = static_cast<SHORT>(HIWORD(wParam));
            auto scrollScale = static_cast<float>(_getWheelScrollLines());
            auto scrollValue = scrollScale * static_cast<float>(nativeScroll) / static_cast<float>(WHEEL_DELTA);
            int modifiers = _getModifiers();

            // NOTE: scroll direction may differ from macOS or Linux
            JNILocal<jobject> eventMouseScroll(env, classes::EventMouseScroll::make(env, -scrollValue, 0.0f, modifiers));
            dispatch(eventMouseScroll.get());
            return 0;
        }

        case WM_SETCURSOR: {
            if (LOWORD(lParam) == HTCLIENT) {
                _setMouseCursorInternal();
                return true;
            }

            break;
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
        case WM_IME_KEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:
        case WM_IME_KEYUP: {
            bool isPressed = !(HIWORD(lParam) & KF_UP);
            int keycode = (int) wParam;
            int modifiers = _getModifiers();
            auto& table = _windowManager.getKeyTable();
            auto& ignoreList = _windowManager.getKeyIgnoreList();

            if (ignoreList.find(keycode) != ignoreList.end())
                break;

            auto mapping = table.find(keycode);
            Key key = mapping != table.end()? mapping->second: Key::UNDEFINED;

            JNILocal<jobject> eventKey(env, classes::EventKey::make(env, key, isPressed, modifiers));
            dispatch(eventKey.get());
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

        case WM_IME_STARTCOMPOSITION:
            JWM_VERBOSE("Start IME composition");

            // Reset cache for safety
            _compositionStr.resize(0);
            _compositionPos = 0;
            _imeChangeCursorPos();
            break;

        case WM_IME_CHAR:
            return 0;

        case WM_IME_COMPOSITION: {
            int iParam = (int)lParam;

            HIMC hImc = ImmGetContext(getHWnd());

            if (!hImc)
                return false;

            if (iParam & (GCS_COMPSTR | GCS_COMPATTR | GCS_CURSORPOS)) {
                // Read partial composition result
                // NOTE: accessible in WM_IME_COMPOSITION or WM_IME_STARTCOMPOSITION message.
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

                if (!selectedFrom && !selectedTo) {
                    selectedFrom = _compositionPos;
                    selectedTo = static_cast<int>(_compositionStr.length());
                }

                auto jcharText = reinterpret_cast<const jchar*>(_compositionStr.c_str());
                auto jlength = static_cast<jsize>(_compositionStr.length());

                JNILocal<jstring> text(env, env->NewString(jcharText, jlength));
                JNILocal<jobject> eventTextInputMarked(env, classes::EventTextInputMarked::make(env, text.get(), selectedFrom, selectedTo));
                dispatch(eventTextInputMarked.get());
            }

            if (iParam & (GCS_RESULTSTR)) {
                // If composition result, we must read its result string here.
                // NOTE: accessible in WM_IME_COMPOSITION or WM_IME_STARTCOMPOSITION message.
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

        case WM_IME_ENDCOMPOSITION: {
            JWM_VERBOSE("End IME composition");

            // Reset cached objects
            _compositionStr.resize(0);
            _compositionPos = 0;

            // Send commit message. This is required, for example if user suddenly closes ime window
            JNILocal<jstring> text(env, env->NewString(nullptr, 0));
            JNILocal<jobject> eventTextInput(env, classes::EventTextInput::make(env, text.get()));
            dispatch(eventTextInput.get());
            break;
        }

        case WM_IME_REQUEST:
            if (wParam == IMR_QUERYCHARPOSITION) {
                // Query current cursor position
                // If composition starts, Pos will be always 0
                auto sectionStart = static_cast<int>(_compositionPos);
                auto sectionEnd = sectionStart + 0;
                auto uiRect = classes::TextInputClient::getRectForMarkedRange(getJNIEnv(), this->fTextInputClient, sectionStart, sectionEnd);

                // Cursor upper left corner (doc requires baseline, but its enough)
                POINT cursorPos;
                cursorPos.x = uiRect.fLeft;
                cursorPos.y = uiRect.fTop;
                ClientToScreen(getHWnd(), &cursorPos);

                // Area of the window (interpreted as document area (where we can place ime window))
                RECT documentArea;
                GetWindowRect(getHWnd(), &documentArea);

                // Fill lParam structure
                // its content will be read after this proc function returns
                auto* imeCharPos = reinterpret_cast<IMECHARPOSITION*>(lParam);
                imeCharPos->dwCharPos = _compositionPos;
                imeCharPos->cLineHeight = uiRect.fBottom - uiRect.fTop;
                imeCharPos->pt = cursorPos;
                imeCharPos->rcDocument = documentArea;
                return true;
            }
            break;

        case WM_CLOSE:
            JWM_VERBOSE("Event close");
            dispatch(classes::EventWindowCloseRequest::kInstance);
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
        JWM_LOG("Failed to init WindowWin32");
        return false;
    }

    // Set this as property to reference from message callbacks
    SetPropW(_hWnd, L"JWM", this);

    // Register window, so manager can process its update
    _windowManager._registerWindow(*this);

    // Set active focus
    SetFocus(_hWnd);

    return true;
}

void jwm::WindowWin32::_destroyInternal() {
    // Remove window from manager and destroy OS object
    _windowManager._unregisterWindow(*this);

    if (_hMouseCursor) {
        DestroyCursor(_hMouseCursor);
        _hMouseCursor = nullptr;
    }

    if (_hWnd) {
        DestroyWindow(_hWnd);
        _hWnd = nullptr;
    }
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

void jwm::WindowWin32::_setMouseCursorInternal() {
    SetCursor(_hMouseCursor);
}

void jwm::WindowWin32::_imeResetComposition() {
    HIMC hImc = ImmGetContext(getHWnd());

    if (!hImc)
        return;

    ImmNotifyIME(hImc, NI_COMPOSITIONSTR, CPS_CANCEL, 0);
    ImmReleaseContext(getHWnd(), hImc);

    _compositionStr.resize(0);
    _compositionPos = 0;
}

void jwm::WindowWin32::_imeChangeCursorPos() const {
    auto sectionStart = static_cast<int>(_compositionPos);
    auto sectionEnd = sectionStart + 0;
    auto uiRect = classes::TextInputClient::getRectForMarkedRange(getJNIEnv(), this->fTextInputClient, sectionStart, sectionEnd);

    HIMC hImc = ImmGetContext(getHWnd());

    if (!hImc)
        return;

    COMPOSITIONFORM compositionform;
    compositionform.dwStyle = CFS_FORCE_POSITION;
    compositionform.ptCurrentPos.x = uiRect.fLeft;
    compositionform.ptCurrentPos.y = uiRect.fBottom;

    CANDIDATEFORM candidateform;
    candidateform.dwIndex = 0;
    candidateform.dwStyle = CFS_EXCLUDE;
    candidateform.ptCurrentPos.x = uiRect.fLeft;;
    candidateform.ptCurrentPos.y = uiRect.fTop;
    candidateform.rcArea.left = uiRect.fLeft;
    candidateform.rcArea.top = uiRect.fTop;
    candidateform.rcArea.right = uiRect.fRight;
    candidateform.rcArea.bottom = uiRect.fBottom;

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

extern "C" JNIEXPORT jobject JNICALL Java_org_jetbrains_jwm_WindowWin32__1nSetTextInputEnabled
        (JNIEnv* env, jobject obj, jboolean enabled) {
    jwm::WindowWin32* instance = reinterpret_cast<jwm::WindowWin32*>(jwm::classes::Native::fromJava(env, obj));
    instance->setImeEnabled(enabled);
    return obj;
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowWin32__1nUnmarkText
        (JNIEnv* env, jobject obj) {
    jwm::WindowWin32* instance = reinterpret_cast<jwm::WindowWin32*>(jwm::classes::Native::fromJava(env, obj));
    instance->unmarkText();
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowWin32__1nShow
        (JNIEnv* env, jobject obj) {
    jwm::WindowWin32* instance = reinterpret_cast<jwm::WindowWin32*>(jwm::classes::Native::fromJava(env, obj));
    instance->show();
}

extern "C" JNIEXPORT jobject JNICALL Java_org_jetbrains_jwm_WindowWin32__1nGetWindowRect
        (JNIEnv* env, jobject obj) {
    jwm::WindowWin32* instance = reinterpret_cast<jwm::WindowWin32*>(jwm::classes::Native::fromJava(env, obj));
    jwm::UIRect rect = instance->getWindowRect();
    return jwm::classes::UIRect::toJava(env, rect);
}

extern "C" JNIEXPORT jobject JNICALL Java_org_jetbrains_jwm_WindowWin32__1nGetContentRect
        (JNIEnv* env, jobject obj) {
    jwm::WindowWin32* instance = reinterpret_cast<jwm::WindowWin32*>(jwm::classes::Native::fromJava(env, obj));
    jwm::UIRect rect = instance->getContentRect();
    return jwm::classes::UIRect::toJava(env, rect);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowWin32__1nSetWindowPosition
        (JNIEnv* env, jobject obj, int left, int top) {
    jwm::WindowWin32* instance = reinterpret_cast<jwm::WindowWin32*>(jwm::classes::Native::fromJava(env, obj));
    instance->setWindowPosition(left, top);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowWin32__1nSetWindowSize
        (JNIEnv* env, jobject obj, int width, int height) {
    jwm::WindowWin32* instance = reinterpret_cast<jwm::WindowWin32*>(jwm::classes::Native::fromJava(env, obj));
    instance->setWindowSize(width, height);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowWin32__1nSetContentSize
        (JNIEnv* env, jobject obj, int width, int height) {
    jwm::WindowWin32* instance = reinterpret_cast<jwm::WindowWin32*>(jwm::classes::Native::fromJava(env, obj));
    instance->setContentSize(width, height);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowWin32__1nSetTitle
        (JNIEnv* env, jobject obj, jstring title) {
    jwm::WindowWin32* instance = reinterpret_cast<jwm::WindowWin32*>(jwm::classes::Native::fromJava(env, obj));
    const jchar* titleStr = env->GetStringChars(title, nullptr);
    jsize length = env->GetStringLength(title);
    instance->setTitle(std::wstring(reinterpret_cast<const wchar_t*>(titleStr), length));
    env->ReleaseStringChars(title, titleStr);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowWin32__1nSetMouseCursor
        (JNIEnv* env, jobject obj, jint cursorId) {
    jwm::WindowWin32* instance = reinterpret_cast<jwm::WindowWin32*>(jwm::classes::Native::fromJava(env, obj));
    instance->setMouseCursor(static_cast<jwm::MouseCursor>(cursorId));
}

extern "C" JNIEXPORT jobject JNICALL Java_org_jetbrains_jwm_WindowWin32__1nGetScreen
        (JNIEnv* env, jobject obj) {
    jwm::WindowWin32* instance = reinterpret_cast<jwm::WindowWin32*>(jwm::classes::Native::fromJava(env, obj));
    jwm::ScreenWin32 screen = instance->getScreen();
    return screen.toJni(env);
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowWin32__1nRequestFrame
        (JNIEnv* env, jobject obj) {
    jwm::WindowWin32* instance = reinterpret_cast<jwm::WindowWin32*>(jwm::classes::Native::fromJava(env, obj));
    instance->requestFrame();
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowWin32__1nClose
        (JNIEnv* env, jobject obj) {
    jwm::WindowWin32* instance = reinterpret_cast<jwm::WindowWin32*>(jwm::classes::Native::fromJava(env, obj));
    instance->close();
}