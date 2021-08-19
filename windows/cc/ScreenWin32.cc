#include <ScreenWin32.hh>
#include <impl/Library.hh>

jobject jwm::ScreenWin32::toJni(JNIEnv* env) const {
    auto id = reinterpret_cast<jlong>(hMonitor);
    return jwm::classes::Screen::make(env, id, x, y, width, height, scale, isPrimary);
}

jwm::ScreenWin32 jwm::ScreenWin32::fromHMonitor(HMONITOR monitor) {
    ScreenWin32 screen{};
    screen.hMonitor = monitor;

    MONITORINFO monitorInfo;
    ZeroMemory(&monitorInfo, sizeof(monitorInfo));
    monitorInfo.cbSize = sizeof(monitorInfo);

    GetMonitorInfoW(monitor, &monitorInfo);
    auto& area = monitorInfo.rcMonitor;

    // Position
    screen.x = area.left;
    screen.y = area.top;

    // Size
    screen.width = area.right - area.left;
    screen.height = area.bottom - area.top;

    // Scale
    DEVICE_SCALE_FACTOR scaleFactor;
    GetScaleFactorForMonitor(monitor, &scaleFactor);

    if (scaleFactor == DEVICE_SCALE_FACTOR_INVALID)
        scaleFactor = JWM_DEFAULT_DEVICE_SCALE;

    screen.scale = (float) scaleFactor / (float) SCALE_100_PERCENT;

    // Is primary
    screen.isPrimary = monitorInfo.dwFlags & MONITORINFOF_PRIMARY;

    return screen;
}
