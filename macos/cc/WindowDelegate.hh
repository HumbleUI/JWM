#pragma once
#import <Cocoa/Cocoa.h>
#include "WindowMac.hh"

@interface WindowDelegate : NSObject<NSWindowDelegate>

- (WindowDelegate*)initWithWindow:(jwm::WindowMac*)initWindow;

@end
