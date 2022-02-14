#pragma once

#import <Cocoa/Cocoa.h>
#include <jni.h>
#import <jni.h>


// http://trac.wxwidgets.org/ticket/13557
// here we subclass NSApplication, for the purpose of being able to override sendEvent
@interface JWMNSApplication: NSApplication {
}

- (id)init;

- (void)sendEvent:(NSEvent *)anEvent;

@end



@interface ApplicationDelegate : NSObject<NSApplicationDelegate> {
    JavaVM *jvm;
    jobject launcher;
}

- (id)initWithJVM:(JavaVM *)vm andLauncherGlobalRef:(jobject)launcher;

// Start the application, blocking until termination.
- (void) runLoop;

- (void)applicationDidFinishLaunching:(NSNotification *)notification;

@end