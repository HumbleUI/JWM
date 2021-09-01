#pragma once
#include <PlatformWin32.hh>

#include "ThemeHelper.hh"
#include "ThemeHelperWin32.hh"
namespace jwm {
    namespace SystemWin32 {
        typedef NTSTATUS(WINAPI *RtlGetVersion_FUNC)(OSVERSIONINFOW *);
        OSVERSIONINFOW getOSVersion();
    }  // namespace SystemWin32
}  // namespace jwm
