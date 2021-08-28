#include <PlatformWin32.hh>
#include "ThemeHelper.hh"
#include "ThemeHelperWin32.hh"
#include <Uxtheme.h>
#include <WinUser.h>
#include <Log.hh>
#include <stdio.h>




jwm::Theme jwm::ThemeHelperWin32::getCurrentTheme() {
  if(!_isDarkModeSupported()){
    JWM_VERBOSE("Dark mode is not supported.");
    return jwm::Theme::Light;
  }
  HMODULE hUxTheme = LoadLibrary(TEXT("uxtheme.dll"));
  if(!hUxTheme){
    JWM_VERBOSE("uxtheme.dll not found.");
    return jwm::Theme::Light;
  }
  jwm::ShouldAppsUseDarkMode hFunc;
  hFunc = (ShouldAppsUseDarkMode)GetProcAddress(hUxTheme, MAKEINTRESOURCEA(132));
  if(hFunc==nullptr){
    JWM_VERBOSE("shouldAppsUseDarkMode not found.");
    FreeLibrary(hUxTheme);
    return jwm::Theme::Light;
  }
  bool shouldAppsUseDarkMode;
  shouldAppsUseDarkMode = hFunc();
  FreeLibrary(hUxTheme);
  return shouldAppsUseDarkMode ? jwm::Theme::Dark : jwm::Theme::Light;
}

void jwm::ThemeHelperWin32::setTheme(jwm::Theme theme) {
  // impl me!
}

// check if OS is Windows 10 and build version >= 17763
bool jwm::ThemeHelperWin32::_isDarkModeSupported() {
  HMODULE hMod;
  jwm::RtlGetVersion_FUNC func;
  hMod = LoadLibrary(TEXT("ntdll.dll"));
  if(hMod){
    func = (jwm::RtlGetVersion_FUNC)GetProcAddress(hMod, "RtlGetVersion");
    if(func == 0){
      JWM_VERBOSE("Failed to load RtlGetVersion function from ntdll.dll");
      FreeLibrary(hMod);
      return false;
    }
    NTSTATUS status;
    OSVERSIONINFOW _osVersionInfo;
    OSVERSIONINFOW * _osVersionInfoPtr = &_osVersionInfo;
    ZeroMemory(_osVersionInfoPtr, sizeof(*_osVersionInfoPtr));
    _osVersionInfoPtr->dwOSVersionInfoSize = sizeof(*_osVersionInfoPtr);
    status = func(&_osVersionInfo);
    FreeLibrary(hMod);
    if(status!=0){
        JWM_VERBOSE("Failed to get osVersionInfo");
      return false;
    }
    JWM_VERBOSE("get os version: '" << _osVersionInfo.dwMajorVersion << "." << _osVersionInfo.dwMinorVersion << "-" <<_osVersionInfo.dwBuildNumber << "'");
    return _osVersionInfo.dwMajorVersion == 10 && _osVersionInfo.dwMinorVersion == 0 && _osVersionInfo.dwBuildNumber >= 17763;
  }
  JWM_VERBOSE("ntdll.dll not found");
  return false;
}