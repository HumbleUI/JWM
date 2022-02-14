#include "ApplicationDelegate.hh"

#include "MainView.hh"
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



@implementation ApplicationDelegate {

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

    JNIEnv *env;
    JavaVMAttachArgs vmAttachArgs = {JNI_VERSION_1_8, 0, 0};
    jint ret = self->jvm->AttachCurrentThread((void**) &env, &vmAttachArgs);
    if (ret == JNI_OK) {
        jwm::classes::Runnable::run(env, self->launcher);
        jwm::classes::Throwable::exceptionThrown(env);
        env->DeleteGlobalRef(self->launcher);
        self->launcher = nullptr;
    } else
        std::cerr << "Failed to AttachCurrentThread: " << ret << std::endl;

    [NSApp run];
    self->jvm->DetachCurrentThread();
}

- (id)initWithJVM:(JavaVM *)pVm andLauncher:(jobject)launcher {
    return nil;
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
