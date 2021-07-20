#include <AppWin32.hh>
#include <WindowManagerWin32.hh>
#include <WindowWin32.hh>
#include <impl/Library.hh>

static LRESULT CALLBACK windowMessageProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    jwm::WindowWin32* window = reinterpret_cast<jwm::WindowWin32*>(GetPropW(hWnd, L"JWM"));

    if (!window) {
        // Handle messages for hidden helper window
        return DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }

    return window->processEvent(uMsg, wParam, lParam);
}

bool jwm::WindowManagerWin32::init() {
    if (!_registerWindowClass())
        return false;
    if (!_createHelperWindow())
        return false;

    _initKeyTable();

    return true;
}

int jwm::WindowManagerWin32::iteration() {
    MSG msg;

    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
        HWND hWnd = msg.hwnd;
        auto iter = _windows.find(hWnd);
        WindowWin32* window = iter != _windows.end()? iter->second: nullptr;

        if (window)
            window->notifyEvent();

        if (msg.message == WM_QUIT) {
            // post close event to managed windows?
        }
        else {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    // Process frame request
    for (auto& entry: _windows) {
        auto& window = entry.second;

        if (window->getFlag(WindowWin32::Flag::RequestFrame)) {
            window->removeFlag(WindowWin32::Flag::RequestFrame);
            window->notifyEvent();
            window->dispatch(classes::EventFrame::kInstance);
        }
    }

    return 0;
}

int jwm::WindowManagerWin32::_registerWindowClass() {
    WNDCLASSEXW wndclassexw;

    ZeroMemory(&wndclassexw, sizeof(wndclassexw));
    wndclassexw.cbSize        = sizeof(wndclassexw);
    wndclassexw.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wndclassexw.lpfnWndProc   = (WNDPROC) windowMessageProc;
    wndclassexw.hInstance     = GetModuleHandleW(NULL);
    wndclassexw.hCursor       = LoadCursorA(NULL, IDC_ARROW);
    wndclassexw.lpszClassName = JWM_WIN32_WINDOW_CLASS_NAME;

    // Icon provided by user
    wndclassexw.hIcon = static_cast<HICON>(LoadImageW(GetModuleHandleW(NULL),
                                                      L"JWM_ICON", IMAGE_ICON,
                                                      0, 0, LR_DEFAULTSIZE | LR_SHARED));
    if (!wndclassexw.hIcon) {
        // Default icon
        wndclassexw.hIcon = static_cast<HICON>(LoadImageA(NULL,
                                                          IDI_APPLICATION, IMAGE_ICON,
                                                          0, 0, LR_DEFAULTSIZE | LR_SHARED));
    }

    if (!RegisterClassExW(&wndclassexw)) {
        AppWin32::getInstance().sendError("Failed to register window class");
        return false;
    }

    return true;
}

int jwm::WindowManagerWin32::_createHelperWindow() {
    _hWndHelperWindow = CreateWindowExW(WS_EX_OVERLAPPEDWINDOW,
                                        JWM_WIN32_WINDOW_CLASS_NAME,
                                        JWM_WIN32_WINDOW_DEFAULT_NAME,
                                        WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                                        0, 0, 1, 1,
                                        NULL, NULL,
                                        GetModuleHandleW(NULL),
                                        NULL);

    if (!_hWndHelperWindow) {
        AppWin32::getInstance().sendError("Failed to create helper window");
        return false;
    }

    ShowWindow(_hWndHelperWindow, SW_HIDE);

    MSG msg;

    while (PeekMessageW(&msg, _hWndHelperWindow, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return true;
}

void jwm::WindowManagerWin32::_initKeyTable() {
    _keyTable[VK_RETURN] = Key::ENTER;
    _keyTable[VK_BACK] = Key::BACK_SPACE;
    _keyTable[VK_TAB] = Key::TAB;
    _keyTable[VK_CANCEL] = Key::CANCEL;
    _keyTable[VK_CLEAR] = Key::CLEAR;
    _keyTable[VK_SHIFT] = Key::SHIFT;
    _keyTable[VK_CONTROL] = Key::CONTROL;
    _keyTable[VK_MENU] = Key::ALT;
    _keyTable[VK_PAUSE] = Key::PAUSE;
    _keyTable[VK_CAPITAL] = Key::CAPS;
    _keyTable[VK_ESCAPE] = Key::ESCAPE;
    _keyTable[VK_SPACE] = Key::SPACE;
    _keyTable[VK_PRIOR] = Key::PAGE_UP;
    _keyTable[VK_NEXT] = Key::PAGE_DOWN;
    _keyTable[VK_END] = Key::END;
    _keyTable[VK_HOME] = Key::HOME;
    _keyTable[VK_LEFT] = Key::LEFT;
    _keyTable[VK_UP] = Key::UP;
    _keyTable[VK_RIGHT] = Key::RIGHT;
    _keyTable[VK_DOWN] = Key::DOWN;
    _keyTable[VK_OEM_COMMA] = Key::COMMA;
    _keyTable[VK_OEM_MINUS] = Key::MINUS;
    _keyTable[VK_OEM_PERIOD] = Key::PERIOD;
    _keyTable[VK_OEM_2] = Key::SLASH;

    _keyTable[0x30] = Key::DIGIT0;
    _keyTable[0x31] = Key::DIGIT1;
    _keyTable[0x32] = Key::DIGIT2;
    _keyTable[0x33] = Key::DIGIT3;
    _keyTable[0x34] = Key::DIGIT4;
    _keyTable[0x35] = Key::DIGIT5;
    _keyTable[0x36] = Key::DIGIT6;
    _keyTable[0x37] = Key::DIGIT7;
    _keyTable[0x38] = Key::DIGIT8;
    _keyTable[0x39] = Key::DIGIT9;

    _keyTable[VK_OEM_1] = Key::SEMICOLON; // ?
    _keyTable[VK_OEM_PLUS] = Key::EQUALS; // ?

    _keyTable[0x41] = Key::A;
    _keyTable[0x42] = Key::B;
    _keyTable[0x43] = Key::C;
    _keyTable[0x44] = Key::D;
    _keyTable[0x45] = Key::E;
    _keyTable[0x46] = Key::F;
    _keyTable[0x47] = Key::G;
    _keyTable[0x48] = Key::H;
    _keyTable[0x49] = Key::I;
    _keyTable[0x4A] = Key::J;
    _keyTable[0x4B] = Key::K;
    _keyTable[0x4C] = Key::L;
    _keyTable[0x4D] = Key::M;
    _keyTable[0x4E] = Key::N;
    _keyTable[0x4F] = Key::O;
    _keyTable[0x50] = Key::P;
    _keyTable[0x51] = Key::Q;
    _keyTable[0x52] = Key::R;
    _keyTable[0x53] = Key::S;
    _keyTable[0x54] = Key::T;
    _keyTable[0x55] = Key::U;
    _keyTable[0x56] = Key::V;
    _keyTable[0x57] = Key::W;
    _keyTable[0x58] = Key::X;
    _keyTable[0x59] = Key::Y;
    _keyTable[0x5A] = Key::Z;

    _keyTable[VK_OEM_4] = Key::OPEN_BRACKET;
    _keyTable[VK_OEM_5] = Key::BACK_SLASH;
    _keyTable[VK_OEM_6] = Key::CLOSE_BRACKET;

    _keyTable[VK_NUMPAD0] = Key::NUMPAD0;
    _keyTable[VK_NUMPAD1] = Key::NUMPAD1;
    _keyTable[VK_NUMPAD2] = Key::NUMPAD2;
    _keyTable[VK_NUMPAD3] = Key::NUMPAD3;
    _keyTable[VK_NUMPAD4] = Key::NUMPAD4;
    _keyTable[VK_NUMPAD5] = Key::NUMPAD5;
    _keyTable[VK_NUMPAD6] = Key::NUMPAD6;
    _keyTable[VK_NUMPAD7] = Key::NUMPAD7;
    _keyTable[VK_NUMPAD8] = Key::NUMPAD8;
    _keyTable[VK_NUMPAD9] = Key::NUMPAD9;

    _keyTable[VK_MULTIPLY] = Key::MULTIPLY;
    _keyTable[VK_ADD] = Key::ADD;
    _keyTable[VK_SEPARATOR] = Key::SEPARATOR;
    _keyTable[VK_SUBTRACT] = Key::SUBTRACT;
    _keyTable[VK_DECIMAL] = Key::DECIMAL;
    _keyTable[VK_DIVIDE] = Key::DIVIDE;
    _keyTable[VK_DELETE] = Key::DEL;
    _keyTable[VK_NUMLOCK] = Key::NUM_LOCK;
    _keyTable[VK_SCROLL] = Key::SCROLL_LOCK;

    _keyTable[VK_F1] = Key::F1;
    _keyTable[VK_F2] = Key::F2;
    _keyTable[VK_F3] = Key::F3;
    _keyTable[VK_F4] = Key::F4;
    _keyTable[VK_F5] = Key::F5;
    _keyTable[VK_F6] = Key::F6;
    _keyTable[VK_F7] = Key::F7;
    _keyTable[VK_F8] = Key::F8;
    _keyTable[VK_F9] = Key::F9;
    _keyTable[VK_F10] = Key::F10;
    _keyTable[VK_F11] = Key::F11;
    _keyTable[VK_F12] = Key::F12;
    _keyTable[VK_F13] = Key::F13;
    _keyTable[VK_F14] = Key::F14;
    _keyTable[VK_F15] = Key::F15;
    _keyTable[VK_F16] = Key::F16;
    _keyTable[VK_F17] = Key::F17;
    _keyTable[VK_F18] = Key::F18;
    _keyTable[VK_F19] = Key::F19;
    _keyTable[VK_F20] = Key::F20;
    _keyTable[VK_F21] = Key::F21;
    _keyTable[VK_F22] = Key::F22;
    _keyTable[VK_F23] = Key::F23;
    _keyTable[VK_F24] = Key::F24;

    _keyTable[VK_SNAPSHOT] = Key::PRINTSCREEN;
    _keyTable[VK_INSERT] = Key::INSERT;
    _keyTable[VK_HELP] = Key::HELP;
    // _keyTable[VK_OEM_3] = Key::META;
    _keyTable[VK_OEM_3] = Key::BACK_QUOTE;
    _keyTable[VK_OEM_7] = Key::QUOTE;

    // KP_UP,
    // KP_DOWN,
    // KP_LEFT,
    // KP_RIGHT,
    // DEAD_GRAVE,
    // DEAD_ACUTE,
    // DEAD_CIRCUMFLEX,
    // DEAD_TILDE,
    // DEAD_MACRON,
    // DEAD_BREVE,
    // DEAD_ABOVEDOT,
    // DEAD_DIAERESIS,
    // DEAD_ABOVERING,
    // DEAD_DOUBLEACUTE,
    // DEAD_CARON,
    // DEAD_CEDILLA,
    // DEAD_OGONEK,
    // DEAD_IOTA,
    // DEAD_VOICED_SOUND,
    // DEAD_SEMIVOICED_SOUND,
    // AMPERSAND,
    // ASTERISK,
    // QUOTEDBL,
    // LESS,
    // GREATER,
    // BRACELEFT,
    // BRACERIGHT,
    //
    // AT,
    // COLON,
    // CIRCUMFLEX,
    // DOLLAR,
    // EURO_SIGN,
    // EXCLAMATION_MARK,
    // INVERTED_EXCLAMATION_MARK,
    // LEFT_PARENTHESIS,
    // NUMBER_SIGN,
    // PLUS,
    // RIGHT_PARENTHESIS,
    // UNDERSCORE,

    _keyTable[VK_LWIN] = Key::WINDOWS;
    _keyTable[VK_RWIN] = Key::WINDOWS;

    // CONTEXT_MENU,
    //
    // FINAL,
    // CONVERT,
    // NONCONVERT,
    // ACCEPT,
    // MODECHANGE,
    // KANA,
    // KANJI,
    // ALPHANUMERIC,
    // KATAKANA,
    // HIRAGANA,
    // FULL_WIDTH,
    // HALF_WIDTH,
    // ROMAN_CHARACTERS,
    // ALL_CANDIDATES,
    // PREVIOUS_CANDIDATE,
    // CODE_INPUT,
    // JAPANESE_KATAKANA,
    // JAPANESE_HIRAGANA,
    // JAPANESE_ROMAN,
    // KANA_LOCK,
    // INPUT_METHOD_ON_OFF,
    // CUT,
    // COPY,
    // PASTE,
    // UNDO,
    // AGAIN,
    // FIND,
    // PROPS,
    // STOP,
    // COMPOSE,
    // ALT_GRAPH,
    // BEGIN,

    // Mobile and Embedded Specific Key Codes
    // SOFTKEY_0,
    // SOFTKEY_1,
    // SOFTKEY_2,
    // SOFTKEY_3,
    // SOFTKEY_4,
    // SOFTKEY_5,
    // SOFTKEY_6,
    // SOFTKEY_7,
    // SOFTKEY_8,
    // SOFTKEY_9,
    // GAME_A,
    // GAME_B,
    // GAME_C,
    // GAME_D,
    // STAR,
    // POUND,
    // POWER,
    // INFO,
    // COLORED_KEY_0,
    // COLORED_KEY_1,
    // COLORED_KEY_2,
    // COLORED_KEY_3,
    // EJECT_TOGGLE,
    // PLAY,
    // RECORD,
    // FAST_FWD,
    // REWIND,
    // TRACK_PREV,
    // TRACK_NEXT,
    // CHANNEL_UP,
    // CHANNEL_DOWN,
    // VOLUME_UP,
    // VOLUME_DOWN,
    // MUTE,
    // COMMAND,
    // FUNCTION,
    // SHORTCUT
}

void jwm::WindowManagerWin32::_registerWindow(class WindowWin32& window) {
    _windows.emplace(window.getHWnd(), &window);
}

void jwm::WindowManagerWin32::_unregisterWindow(class WindowWin32& window) {
    _windows.erase(window.getHWnd());
}
