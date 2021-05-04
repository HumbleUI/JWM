#pragma once
#import <Cocoa/Cocoa.h>
#include "Context.hh"
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

namespace jwm {
    class MetalContext: public Context {
    public:
        MetalContext(bool vsync): fVsync(vsync) {}
        ~MetalContext();

        void attach(NSView* mainView);
        void swapBuffers() override;
        void resize() override;

        int fWidth = 0;
        int fHeight = 0;
        int fSampleCount = 1;
        int fStencilBits = 8;
        bool fVsync = true;
        bool                fValid = false;
        id<MTLDevice>       fDevice;
        id<MTLCommandQueue> fQueue;
        CAMetalLayer*       fMetalLayer;
        NSView*             fMainView;
        id<CAMetalDrawable> fDrawableHandle;
        // id<MTLBinaryArchive> fPipelineArchive API_AVAILABLE(macos(11.0), ios(14.0));
    };

    void deleteMetalContext(MetalContext* instance);
}