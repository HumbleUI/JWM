#include <iostream>
#include <jni.h>
#include "impl/Library.hh"
#include "WindowMac.hh"
#include "MainView.hh"

@implementation MainView {
    jwm::WindowMac* fWindow;
    // A TrackingArea prevents us from capturing events outside the view
    NSTrackingArea* fTrackingArea;
    // We keep track of the state of the modifier keys on each event in order to synthesize
    // key-up/down events for each modifier.
    jwm::ModifierKey fLastModifiers;
    id<MTLDevice> fDevice;
    id<MTLCommandQueue> fQueue;
}

- (MainView*)initWithWindow:(jwm::WindowMac*)initWindow frame:(CGRect)frameRect {
    fDevice = MTLCreateSystemDefaultDevice();
    fQueue = [fDevice newCommandQueue];
    self = [super initWithFrame:frameRect device:fDevice];

    [self setDepthStencilPixelFormat:MTLPixelFormatDepth32Float_Stencil8];
    [self setColorPixelFormat:MTLPixelFormatBGRA8Unorm];
    [self setSampleCount:1];

    self.delegate = self;
    self.paused = NO;
    self.enableSetNeedsDisplay = NO;
    self.needsDisplay = YES;
    self.preferredFramesPerSecond = 120;
    // self.presentsWithTransaction = YES;

    fWindow = initWindow;
    fTrackingArea = nil;

    [self updateTrackingAreas];

    return self;
}

int now() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

- (void)drawInMTKView:(nonnull MTKView *)view {
    std::cout << "drawInMTKView " << now() << std::endl;

    MTLRenderPassDescriptor* desc = view.currentRenderPassDescriptor;
    fWindow->onEvent(jwm::classes::EventPaint::kInstance);

    id<MTLCommandBuffer> buffer([fQueue commandBuffer]);
    id<MTLRenderCommandEncoder> encoder = [buffer renderCommandEncoderWithDescriptor:desc];

    [encoder endEncoding];
    [buffer presentDrawable:view.currentDrawable];
    [buffer commit];
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size {
    std::cout << "drawableSizeWillChange " << now() << " size=" << size.width << "x" << size.height << std::endl;
    self.needsDisplay = YES;
}

- (id<MTLDevice>)getDevice {
    return fDevice;
}

- (id<MTLCommandQueue>)getQueue {
    return fQueue;
}


- (void)dealloc {
    [fTrackingArea release];
    [super dealloc];
}

- (BOOL)isOpaque {
    return YES;
}

- (BOOL)canBecomeKeyView {
    return YES;
}

- (BOOL)acceptsFirstResponder {
    return YES;
}

- (void)updateTrackingAreas {
    if (fTrackingArea != nil) {
        [self removeTrackingArea:fTrackingArea];
        [fTrackingArea release];
    }

    const NSTrackingAreaOptions options = NSTrackingMouseEnteredAndExited |
                                          NSTrackingActiveInKeyWindow |
                                          NSTrackingEnabledDuringMouseDrag |
                                          NSTrackingCursorUpdate |
                                          NSTrackingInVisibleRect |
                                          NSTrackingAssumeInside;

    fTrackingArea = [[NSTrackingArea alloc] initWithRect:[self bounds]
                                                 options:options
                                                   owner:self
                                                userInfo:nil];

    [self addTrackingArea:fTrackingArea];
    [super updateTrackingAreas];
}

- (void)mouseMoved:(NSEvent *)event {
    NSView* view = fWindow->fNSWindow.contentView;
    CGFloat scale = fWindow->scaleFactor();

    // skui::ModifierKey modifiers = [self updateModifierKeys:event];

    const NSPoint pos = [event locationInWindow];
    const NSRect rect = [view frame];
    jwm::AutoLocal<jobject> eventObj(fWindow->fEnv, jwm::classes::EventMouseMove::make(fWindow->fEnv, (jint) (pos.x * scale), (jint) ((rect.size.height - pos.y) * scale)));
    fWindow->onEvent(eventObj.get());
}

- (void)keyDown:(NSEvent *)event {
    jwm::AutoLocal<jobject> eventObj(fWindow->fEnv, jwm::classes::EventKeyboard::make(fWindow->fEnv, (jint) [event keyCode],
                                                                                      (jboolean) true));
    fWindow->onEvent(eventObj.get());
}

- (void)keyUp:(NSEvent *)event {
    jwm::AutoLocal<jobject> eventObj(fWindow->fEnv, jwm::classes::EventKeyboard::make(fWindow->fEnv, (jint) [event keyCode],
                                                                                      (jboolean) false));
    fWindow->onEvent(eventObj.get());
}

@end

CGFloat jwm::backingScaleFactor(NSView* mainView) {
    NSScreen* screen = mainView.window.screen ?: [NSScreen mainScreen];
    return screen.backingScaleFactor;
}
