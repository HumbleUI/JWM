#include "JWMApplicationDelegate.hh"

#include "JWMMainView.hh"
#include "JWMWindowDelegate.hh"
#include "Log.hh"

@implementation JWMNSApplication

- (id)init {
    self = [super init];
    return self;
}

// This is needed because otherwise we don't receive any key-up events for command-key combinations (an AppKit bug, apparently)
- (void)sendEvent:(NSEvent *)anEvent {
    if ([anEvent type] == NSEventTypeKeyUp && ([anEvent modifierFlags] & NSEventModifierFlagCommand))
        [[self keyWindow] sendEvent:anEvent];
    else
        [super sendEvent:anEvent];
}

@end



@implementation JWMApplicationDelegate {

}

- (id)initWithJVM: (JavaVM *)vm andLauncherGlobalRef: (jobject)launcherRef {
    self = [super init];
    self->jvm = vm;
    self->launcher = launcherRef;
    return self;
}

- (void) runLoop {
    [JWMNSApplication sharedApplication];

    [NSApp setDelegate:self];

    jwm::initKeyTable();
    jwm::initCursorCache();

    JNIEnv *env = nullptr;
    bool didAttach = false;
    if (self->jvm->GetEnv((void**) &env, JNI_VERSION_1_8) == JNI_EDETACHED) {
        JavaVMAttachArgs vmAttachArgs = {JNI_VERSION_1_8, 0, 0};
        if (self->jvm->AttachCurrentThread((void**) &env, &vmAttachArgs) == JNI_OK) {
            didAttach = true;
        } else {
            std::cerr << "Failed to AttachCurrentThread" << std::endl;
        }
    }

    if (env != nullptr) {
        jwm::classes::Runnable::run(env, self->launcher);
        jwm::classes::Throwable::exceptionThrown(env);
        env->DeleteGlobalRef(self->launcher);
        self->launcher = nullptr;
    }

    [NSApp run];

    if (didAttach) {
        self->jvm->DetachCurrentThread();
    }
}

- (id)initWithJVM:(JavaVM *)pVm andLauncher:(jobject)launcher {
    return nil;
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender {
    // https://github.com/glfw/glfw/blob/ed6452b13c76f7b4da216a9952bc7837aeb0f031/src/cocoa_init.m#L402-L408
    //
    // Cmd+Q / menu Quit: deliver a close request to every JWM window — the same
    // path as the window's close button — and let the app decide whether to quit.
    // Never return NSTerminateNow: AppKit would call exit() and hard-kill the JVM.
    // The app quits by closing its windows, which eventually calls App.terminate()
    // -> [NSApp stop:], unwinding [NSApp run] for a clean shutdown.

    for (NSWindow* window in [NSApp windows]) {
        id<NSWindowDelegate> delegate = [window delegate];
        if ([delegate isKindOfClass:[JWMWindowDelegate class]]) {
            [delegate windowShouldClose:window];
        }
    }
    return NSTerminateCancel;
}

- (void)applicationDidFinishLaunching:(NSNotification *)notification {
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    [NSApp activateIgnoringOtherApps:YES];

    // Create application menu
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSMenu *menuBar = [[NSMenu alloc] initWithTitle:@""];
    [NSApp setMainMenu:menuBar];

    NSMenuItem *item;
    NSMenu *menu;

    item = [[NSMenuItem alloc] initWithTitle:@"" action:nil keyEquivalent:@""];
    [menuBar addItem:item];

    menu = [[NSMenu alloc] initWithTitle:@"Apple"];
    [menuBar setSubmenu:menu forItem:item];
    [item release];

    item = [[NSMenuItem alloc] initWithTitle:@"Quit" action:@selector(terminate:) keyEquivalent:@"q"];
    [menu addItem:item];
    [item release];

    [menu release];
    [menuBar release];
    [pool release];
}

@end
