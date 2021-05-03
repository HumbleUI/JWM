#pragma once
#import <Cocoa/Cocoa.h>
#include "Window.hh"

@interface MainView : NSView

- (MainView*)initWithWindow:(jwm::Window*)initWindow;

@end

