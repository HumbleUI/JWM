#pragma once
#import <Cocoa/Cocoa.h>
#include "Context.hh"
#include "Window.hh"
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

namespace jwm {
    class ContextMetal: public Context {
    public:
        ContextMetal(bool vsync, bool transact): fVsync(vsync), fTransact(transact) {}
        ~ContextMetal();

        void attach(Window* window) override;
        void swapBuffers() override;
        void resize() override;

        int fWidth = 0;
        int fHeight = 0;
        int fSampleCount = 1;
        int fStencilBits = 8;
        bool fVsync;
        bool fTransact;
        bool                fValid = false;
        id<MTLDevice>       fDevice;
        id<MTLCommandQueue> fQueue;
        CAMetalLayer*       fMetalLayer;
        NSView*             fMainView;
        id<CAMetalDrawable> fDrawableHandle;
        // id<MTLBinaryArchive> fPipelineArchive API_AVAILABLE(macos(11.0), ios(14.0));
    };

    void deleteContextMetal(ContextMetal* instance);
}