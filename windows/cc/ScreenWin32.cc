#include <ScreenWin32.hh>

jobject jwm::ScreenWin32::toJni(JNIEnv* env) const {
    auto id = reinterpret_cast<jlong>(hMonitor);
    return jwm::classes::Screen::make(env, id, isPrimary, bounds, workArea, scale);
}

jwm::ScreenWin32 jwm::ScreenWin32::fromHMonitor(HMONITOR monitor) {
    ScreenWin32 screen{};
    screen.hMonitor = monitor;

    MONITORINFO monitorInfo;
    ZeroMemory(&monitorInfo, sizeof(monitorInfo));
    monitorInfo.cbSize = sizeof(monitorInfo);

    GetMonitorInfoW(monitor, &monitorInfo);
    auto& area = monitorInfo.rcMonitor;

    // Bounds
    screen.bounds.fLeft = area.left;
    screen.bounds.fTop = area.top;
    screen.bounds.fRight = area.right;
    screen.bounds.fBottom = area.bottom;

    // Work area
    auto& workArea = monitorInfo.rcWork;
    screen.workArea.fLeft = workArea.left;
    screen.workArea.fTop = workArea.top;
    screen.workArea.fRight = workArea.right;
    screen.workArea.fBottom = workArea.bottom;

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
