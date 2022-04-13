#pragma once
#import <Cocoa/Cocoa.h>
#include "WindowMac.hh"

@interface JWMWindowDelegate : NSObject<NSWindowDelegate>

- (JWMWindowDelegate*)initWithWindow:(jwm::WindowMac*)initWindow;

@end
