#pragma once
#import <Cocoa/Cocoa.h>

@interface WindowDelegate : NSObject<NSWindowDelegate>

- (WindowDelegate*)initWithWindow:(jwm::Window*)initWindow;

@end
