#pragma once
#import <Cocoa/Cocoa.h>
#include "WindowMac.hh"

@interface MainView : NSView

- (MainView*)initWithWindow:(jwm::WindowMac*)initWindow;

@end
