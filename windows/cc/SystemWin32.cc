#include "SystemWin32.hh"

#include <VersionHelpers.h>
#include <WinUser.h>
#include <stdio.h>

#include <Log.hh>
#include <PlatformWin32.hh>

OSVERSIONINFOW jwm::SystemWin32::getOSVersion() {
    HMODULE hMod;
    jwm::SystemWin32::FnRtlGetVersion fn;
    hMod = LoadLibrary(TEXT("ntdll.dll"));
    if (!hMod) {
        JWM_VERBOSE("ntdll.dll not found");
        throw "LibNotFound";
    }
    fn = (jwm::SystemWin32::FnRtlGetVersion)GetProcAddress(hMod, "RtlGetVersion");
    if (fn == 0) {
        JWM_VERBOSE("Failed to load RtlGetVersion function from ntdll.dll");
        FreeLibrary(hMod);
        throw "FunctionNotFound";
    }
    NTSTATUS status;
    OSVERSIONINFOW _osVersionInfo;
    OSVERSIONINFOW *_osVersionInfoPtr = &_osVersionInfo;
    ZeroMemory(_osVersionInfoPtr, sizeof(*_osVersionInfoPtr));
    _osVersionInfoPtr->dwOSVersionInfoSize = sizeof(*_osVersionInfoPtr);
    status = fn(&_osVersionInfo);
    FreeLibrary(hMod);
    if (status != 0) {
        JWM_VERBOSE("Failed to get osVersionInfo");
        throw "FailedToGetOSVersionInfo";
    }
    JWM_VERBOSE("get os version: '" << _osVersionInfo.dwMajorVersion << "."
                                    << _osVersionInfo.dwMinorVersion << "-"
                                    << _osVersionInfo.dwBuildNumber << "'");
    return _osVersionInfo;
}
