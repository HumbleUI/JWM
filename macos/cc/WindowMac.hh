#pragma once
#import <Cocoa/Cocoa.h>
#include "Context.hh"
#include "Window.hh"
#include <jni.h>

namespace jwm {
    class WindowMac: public Window {
    public:
        WindowMac(JNIEnv* env): Window(env) {}
        ~WindowMac() override;
        bool init() override;
        float scaleFactor() const override;

        NSWindow* fNSWindow = nullptr;
    };
}