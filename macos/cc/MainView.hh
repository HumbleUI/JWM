#pragma once
#import <Cocoa/Cocoa.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#include "WindowMac.hh"

@interface MainView : MTKView<MTKViewDelegate>

- (MainView*)initWithWindow:(jwm::WindowMac*)initWindow frame:(CGRect)frameRect;

- (id<MTLDevice>)getDevice;
- (id<MTLCommandQueue>)getQueue;

@end

namespace jwm {
    CGFloat backingScaleFactor(NSView* mainView);
}
