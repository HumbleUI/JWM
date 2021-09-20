#include <AppWin32.hh>
#include <WindowManagerWin32.hh>
#include <WindowWin32.hh>
#include <Log.hh>

static LRESULT CALLBACK windowMessageProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    jwm::WindowWin32* window = reinterpret_cast<jwm::WindowWin32*>(GetPropW(hWnd, L"JWM"));

    if (!window) {
        jwm::AppWin32& app = jwm::AppWin32::getInstance();
        LRESULT result = app.processEvent(uMsg, wParam, lParam);
        return result? result: DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }

    return window->processEvent(uMsg, wParam, lParam);
}

bool jwm::WindowManagerWin32::init() {
    // NOTE: We report all coords/sizes in pixels
    // Scaling is handled manually by user application
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    if (!_registerWindowClass())
        return false;
    if (!_createHelperWindow())
        return false;

    _initKeyTable();
    _initKeyLocations();
    _initKeyIgnoreList();

    return true;
}

int jwm::WindowManagerWin32::start() {
    MSG msg;

    auto dispatch = [&](){
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    };

    while (GetMessageW(&msg, nullptr, 0, 0)) {
        if (msg.message == WM_CLOSE) {
            // App terminate requested
            // leave event loop and close application
            return 0;
        }
        if (msg.message == JWM_WM_FRAME_EVENT) {
            // Frame requested.
            // Peek all messages and swap buffers.
            break;
        }

        dispatch();
    }

    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_CLOSE) {
            // App terminate requested
            // leave event loop and close application
            return 0;
        }

        dispatch();
    }

    _dispatchFrameEvents();

    return 0;
}

void jwm::WindowManagerWin32::timerUpdate() {
    _dispatchFrameEvents();
}

int jwm::WindowManagerWin32::_registerWindowClass() {
    WNDCLASSEXW wndclassexw;

    ZeroMemory(&wndclassexw, sizeof(wndclassexw));
    wndclassexw.cbSize        = sizeof(wndclassexw);
    wndclassexw.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wndclassexw.lpfnWndProc   = (WNDPROC) windowMessageProc;
    wndclassexw.hInstance     = GetModuleHandleW(nullptr);
    wndclassexw.hCursor       = LoadCursorW(nullptr, IDC_ARROW);
    wndclassexw.lpszClassName = JWM_WIN32_WINDOW_CLASS_NAME;

    // Icon provided by user
    wndclassexw.hIcon = static_cast<HICON>(LoadImageW(GetModuleHandleW(nullptr),
                                                      L"JWM_ICON", IMAGE_ICON,
                                                      0, 0, LR_DEFAULTSIZE | LR_SHARED));
    if (!wndclassexw.hIcon) {
        // Default icon
        wndclassexw.hIcon = static_cast<HICON>(LoadImageW(nullptr,
                                                          IDI_APPLICATION, IMAGE_ICON,
                                                          0, 0, LR_DEFAULTSIZE | LR_SHARED));
    }

    if (!RegisterClassExW(&wndclassexw)) {
        JWM_LOG("Failed to register window class");
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
                                        nullptr, nullptr,
                                        GetModuleHandleW(nullptr),
                                        nullptr);

    if (!_hWndHelperWindow) {
        JWM_LOG("Failed to create helper window");
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
    // Modifiers
    _keyTable[VK_CAPITAL] = Key::CAPS_LOCK;
    _keyTable[VK_SHIFT] = Key::SHIFT;
    _keyTable[VK_CONTROL] = Key::CONTROL;
    _keyTable[VK_MENU] = Key::ALT;
    _keyTable[VK_LWIN] = Key::WIN_LOGO;
    _keyTable[VK_RWIN] = Key::WIN_LOGO;
    // LINUX_META
    // LINUX_SUPER
    // MAC_COMMAND
    // MAC_OPTION
    // MAC_FN

    // Rest of the keys
    _keyTable[VK_RETURN] = Key::ENTER;
    _keyTable[VK_BACK] = Key::BACKSPACE;
    _keyTable[VK_TAB] = Key::TAB;
    _keyTable[VK_CANCEL] = Key::CANCEL;
    _keyTable[VK_CLEAR] = Key::CLEAR;
    _keyTable[VK_PAUSE] = Key::PAUSE;
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

    _keyTable[VK_NUMPAD0] = Key::DIGIT0;
    _keyTable[VK_NUMPAD1] = Key::DIGIT1;
    _keyTable[VK_NUMPAD2] = Key::DIGIT2;
    _keyTable[VK_NUMPAD3] = Key::DIGIT3;
    _keyTable[VK_NUMPAD4] = Key::DIGIT4;
    _keyTable[VK_NUMPAD5] = Key::DIGIT5;
    _keyTable[VK_NUMPAD6] = Key::DIGIT6;
    _keyTable[VK_NUMPAD7] = Key::DIGIT7;
    _keyTable[VK_NUMPAD8] = Key::DIGIT8;
    _keyTable[VK_NUMPAD9] = Key::DIGIT9;

    _keyTable[VK_MULTIPLY] = Key::MULTIPLY;
    _keyTable[VK_ADD] = Key::ADD;
    _keyTable[VK_SEPARATOR] = Key::SEPARATOR;
    _keyTable[VK_SUBTRACT] = Key::MINUS;
    _keyTable[VK_DECIMAL] = Key::PERIOD;
    _keyTable[VK_DIVIDE] = Key::SLASH;
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
    _keyTable[VK_OEM_3] = Key::BACK_QUOTE;
    _keyTable[VK_OEM_7] = Key::QUOTE;
    // Key::MENU
    // Key::KANA
    // Key::VOLUME_UP
    // Key::VOLUME_DOWN
    // Key::MUTE
}

void jwm::WindowManagerWin32::_initKeyLocations() {
    _keyLocations[VK_CONTROL] = KeyLocation::RIGHT;
    _keyLocations[VK_MENU] = KeyLocation::RIGHT;
    _keyLocations[VK_SHIFT] = KeyLocation::RIGHT;
    _keyLocations[VK_RWIN] = KeyLocation::RIGHT;
    _keyLocations[VK_RSHIFT] = KeyLocation::RIGHT;
    _keyLocations[VK_RMENU] = KeyLocation::RIGHT;

    _keyLocations[VK_NUMPAD0] = KeyLocation::KEYPAD;
    _keyLocations[VK_NUMPAD1] = KeyLocation::KEYPAD;
    _keyLocations[VK_NUMPAD2] = KeyLocation::KEYPAD;
    _keyLocations[VK_NUMPAD3] = KeyLocation::KEYPAD;
    _keyLocations[VK_NUMPAD4] = KeyLocation::KEYPAD;
    _keyLocations[VK_NUMPAD5] = KeyLocation::KEYPAD;
    _keyLocations[VK_NUMPAD6] = KeyLocation::KEYPAD;
    _keyLocations[VK_NUMPAD7] = KeyLocation::KEYPAD;
    _keyLocations[VK_NUMPAD8] = KeyLocation::KEYPAD;
    _keyLocations[VK_NUMPAD9] = KeyLocation::KEYPAD;

    _keyLocations[VK_ADD] = KeyLocation::KEYPAD;
    _keyLocations[VK_SEPARATOR] = KeyLocation::KEYPAD;
    _keyLocations[VK_SUBTRACT] = KeyLocation::KEYPAD;
    _keyLocations[VK_MULTIPLY] = KeyLocation::KEYPAD;
    _keyLocations[VK_DIVIDE] = KeyLocation::KEYPAD;
    _keyLocations[VK_RETURN] = KeyLocation::KEYPAD;

    _keyLocations[VK_NUMLOCK] = KeyLocation::KEYPAD;
}

void jwm::WindowManagerWin32::_initKeyIgnoreList() {
    _keyIgnoreList.emplace(VK_PROCESSKEY);
}

void jwm::WindowManagerWin32::_registerWindow(class WindowWin32& window) {
    std::lock_guard<std::mutex> lock(_accessMutex);
    _windows.emplace(window.getHWnd(), &window);
}

void jwm::WindowManagerWin32::_unregisterWindow(class WindowWin32& window) {
    std::lock_guard<std::mutex> lock(_accessMutex);
    _windows.erase(window.getHWnd());
}

void jwm::WindowManagerWin32::_dispatchFrameEvents() {
    // NOTE: now request is processed.
    // If user requests new frame in this method,
    // we will catch it and will handle in the new processing iteration
    _requestFrame = false;

    std::vector<WindowWin32*> windowsGL;
    std::vector<WindowWin32*> windowsD3DorRaster;

    for (auto entry: _windows) {
        auto window = entry.second;

        if (window->testFlag(WindowWin32::Flag::RequestFrame)) {
            window->removeFlag(WindowWin32::Flag::RequestFrame);

            if (window->testFlag(WindowWin32::Flag::HasLayerGL))
                windowsGL.push_back(window);
            else if (window->testFlag(WindowWin32::Flag::HasLayerD3D) ||
                     window->testFlag(WindowWin32::Flag::HasLayerRaster))
                windowsD3DorRaster.push_back(window);
        }
    }

    for (auto window: windowsD3DorRaster) {
        window->notifyEvent(WindowWin32::Event::SwitchContext);
        window->dispatch(classes::EventFrame::kInstance);

        if (window->testFlag(WindowWin32::Flag::RequestSwap)) {
            window->notifyEvent(WindowWin32::Event::SwapBuffers);
            window->removeFlag(WindowWin32::Flag::RequestSwap);
        }
    }

    for (auto window: windowsGL) {
        window->notifyEvent(WindowWin32::Event::SwitchContext);

        if (window == windowsGL.back())
            window->notifyEvent(WindowWin32::Event::EnableVsync);

        window->dispatch(classes::EventFrame::kInstance);

        if (window->testFlag(WindowWin32::Flag::RequestSwap)) {
            window->notifyEvent(WindowWin32::Event::SwapBuffers);
            window->removeFlag(WindowWin32::Flag::RequestSwap);
        }

        if (window == windowsGL.back())
            window->notifyEvent(WindowWin32::Event::DisableVsync);
    }
}

void jwm::WindowManagerWin32::postMessage(UINT messageId, void *lParam) {
    PostMessageW(getHelperWindow(), messageId, 0, reinterpret_cast<LONG_PTR>(lParam));
}

void jwm::WindowManagerWin32::requestFrameEvent() {
    if (!_requestFrame) {
        _requestFrame = true;
        postMessage(JWM_WM_FRAME_EVENT, nullptr);
    }
}
