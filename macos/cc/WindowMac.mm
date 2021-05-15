#include <iostream>
#include <jni.h>
#include "impl/Library.hh"
#include "MainView.hh"
#include <memory>
#include "WindowMac.hh"
#include "WindowDelegate.hh"

static CVReturn displayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* _now, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* ctx) {
    jwm::WindowMac* self = (jwm::WindowMac*) ctx;
    if (self->fNeedRedraw) {
        self->fNeedRedraw = false;
        self->ref();
        dispatch_async(dispatch_get_main_queue(), ^{
            if (self->fContext)
                self->onEvent(jwm::classes::EventPaint::kInstance);
            self->unref();
            self->fNeedRedraw = true;
        });
    }
    return kCVReturnSuccess;
}

jwm::WindowMac::~WindowMac() {
    CVDisplayLinkStop(fDisplayLink);
    CVDisplayLinkRelease(fDisplayLink);
    [fNSWindow close];
}

bool jwm::WindowMac::init() {
    // Create a delegate to track certain events
    WindowDelegate* delegate = [[WindowDelegate alloc] initWithWindow:this];
    if (nil == delegate)
        return false;

    // Create Cocoa window
    constexpr int initialWidth = 800;
    constexpr int initialHeight = 600;
    NSRect windowRect = NSMakeRect(100 + rand() % 100, 100 + rand() % 100, initialWidth, initialHeight);

    NSUInteger windowStyle = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable | NSWindowStyleMaskMiniaturizable;

    fNSWindow = [[NSWindow alloc] initWithContentRect:windowRect styleMask:windowStyle
                                           backing:NSBackingStoreBuffered defer:NO];
    if (nil == fNSWindow) {
        [delegate release];
        return false;
    }

    // create view
    MainView* view = [[MainView alloc] initWithWindow:this];
    if (nil == view) {
        [fNSWindow release];
        [delegate release];
        return false;
    }

    [fNSWindow setContentView:view];
    [fNSWindow makeFirstResponder:view];
    [fNSWindow setDelegate:delegate];
    [fNSWindow setAcceptsMouseMovedEvents:YES];
    [fNSWindow setRestorable:NO];

    // Should be retained by window now
    [view release];

    return true;
}

void jwm::WindowMac::invalidate() {
    if (!fDisplayLink) {
        CVDisplayLinkCreateWithActiveCGDisplays(&fDisplayLink);
        CVDisplayLinkSetOutputCallback(fDisplayLink, &displayLinkCallback, this);
        CVDisplayLinkStart(fDisplayLink);
    }

    CGDirectDisplayID currentDisplay = (CGDirectDisplayID)[[[[fNSWindow screen] deviceDescription] objectForKey:@"NSScreenNumber"] intValue];
    CGDirectDisplayID oldDisplay = CVDisplayLinkGetCurrentCGDisplay(fDisplayLink);
    if (currentDisplay != oldDisplay)
        CVDisplayLinkSetCurrentCGDisplay(fDisplayLink, currentDisplay);
}

float jwm::WindowMac::scaleFactor() const {
    NSScreen* screen = fNSWindow.contentView.window.screen ?: [NSScreen mainScreen];
    return screen.backingScaleFactor;
}


// JNI

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_jwm_WindowMac__1nMake
  (JNIEnv* env, jclass jclass) {
    std::unique_ptr<jwm::WindowMac> instance(new jwm::WindowMac(env));
    if (instance->init())
      return reinterpret_cast<jlong>(instance.release());
    else
      return 0;
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_jwm_WindowMac__1nShow
  (JNIEnv* env, jobject obj) {
    jwm::WindowMac* instance = reinterpret_cast<jwm::WindowMac*>(jwm::classes::Native::fromJava(env, obj));
    [instance->fNSWindow orderFront:nil];
    [NSApp activateIgnoringOtherApps:YES];
    [instance->fNSWindow makeKeyAndOrderFront:NSApp];
    instance->invalidate();
}
