#include <AppWin32.hh>
#include <WindowManagerWin32.hh>
#include <WindowWin32.hh>
#include <impl/Library.hh>
#include <iostream>

static LRESULT CALLBACK windowMessageProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    jwm::WindowWin32* window = reinterpret_cast<jwm::WindowWin32*>(GetPropW(hWnd, L"JWM"));

    switch (uMsg) {
        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                jwm::AppWin32::getInstance().terminate();
            }
            break;

        case WM_CLOSE:
            jwm::AppWin32::getInstance().terminate();
            break;

        default:
            break;
    }

    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

int jwm::WindowManagerWin32::init() {
    if (!_registerWindowClass())
        return JWM_FALSE;
    if (!_createHelperWindow())
        return JWM_FALSE;

    return JWM_TRUE;
}

int jwm::WindowManagerWin32::runMainLoop() {
    while (!_terminateRequested.load()) {
        MSG msg;

        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                // post close event to managed windows?
            } else {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }

        // Dispatch frame request event for those windows,
        // which want to redraw in the next frame
        std::unordered_set<WindowWin32*> toProcess;
        std::swap(toProcess, _frameRequests);

        for (auto window: toProcess) {
            window->dispatch(classes::EventFrame::kInstance);
        }
    }

    return 0;
}

void jwm::WindowManagerWin32::requestTerminate() {
    _terminateRequested.store(true);
}

void jwm::WindowManagerWin32::requestFrame(WindowWin32* window) {
    _frameRequests.emplace(window);
}

void jwm::WindowManagerWin32::sendError(const char *what) {
    std::cerr << "jwm::WindowManagerWin32::Error: " << what << std::endl;
}

int jwm::WindowManagerWin32::_registerWindowClass() {
    WNDCLASSEXW wndclassexw;

    ZeroMemory(&wndclassexw, sizeof(wndclassexw));
    wndclassexw.cbSize        = sizeof(wndclassexw);
    wndclassexw.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
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
        sendError("Failed to register window class");
        return JWM_FALSE;
    }

    return JWM_TRUE;
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
        sendError("Failed to create helper window");
        return JWM_FALSE;
    }

    ShowWindow(_hWndHelperWindow, SW_HIDE);

    MSG msg;

    while (PeekMessageW(&msg, _hWndHelperWindow, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return JWM_TRUE;
}

void jwm::WindowManagerWin32::_registerWindow(class WindowWin32& window) {
    _windows.emplace(&window);
}

void jwm::WindowManagerWin32::_unregisterWindow(class WindowWin32& window) {
    _windows.erase(&window);
}
