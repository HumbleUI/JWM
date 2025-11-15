#include <jni.h>
#include <windows.h>
#include <icm.h>
#include <fstream>
#include <vector>

extern "C" JNIEXPORT jbyteArray JNICALL Java_io_github_humbleui_jwm_Screen__1nGetICCProfile
  (JNIEnv* env, jclass jclass, jlong screenId) {
    HMONITOR monitor = reinterpret_cast<HMONITOR>(screenId);

    // monitor_info
    MONITORINFOEX monitor_info;
    ZeroMemory(&monitor_info, sizeof(monitor_info));
    monitor_info.cbSize = sizeof(monitor_info);
    ::GetMonitorInfo(monitor, &monitor_info);

    // ICC profile path
    std::wstring profile_path;
    HDC hdc = ::CreateDC(monitor_info.szDevice, NULL, NULL, NULL);
    if (hdc) {
        DWORD path_length = MAX_PATH;
        WCHAR path[MAX_PATH + 1];
        BOOL result = ::GetICMProfile(hdc, &path_length, path);
        ::DeleteDC(hdc);
        if (result) {
            profile_path = std::wstring(path);
        }
    }

    if (profile_path.empty()) {
        return nullptr;
    }

    // Read ICC profile file
    std::ifstream file(profile_path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        return nullptr;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        return nullptr;
    }

    // Create Java byte array
    jbyteArray result = env->NewByteArray(static_cast<jsize>(size));
    if (result) {
        env->SetByteArrayRegion(result, 0, static_cast<jsize>(size),
                                reinterpret_cast<const jbyte*>(buffer.data()));
    }

    return result;
}
