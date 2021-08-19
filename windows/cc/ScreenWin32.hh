#pragma once
#include <PlatformWin32.hh>
#include <jni.h>

namespace jwm {

    class ScreenWin32 {
    public:
        jobject toJni(JNIEnv* env) const;

    private:
        ScreenWin32() = default;

    private:
        HMONITOR hMonitor;
        int x;
        int y;
        int width;
        int height;
        float scale;
        bool isPrimary;

    public:
        static ScreenWin32 fromHMonitor(HMONITOR monitor);
    };

}