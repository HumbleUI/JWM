#pragma once
#import <Cocoa/Cocoa.h>
#include "WindowMac.hh"
#include "Key.hh"

namespace jwm {
    extern Key kKeyTable[];
    BOOL isPressAndHoldEnabledGlobally();
    void initKeyTable();
}

@interface JWMMainView : NSView <NSTextInputClient> {
    BOOL fInPressAndHold;
    BOOL fKeyEventsNeeded;
    BOOL fPressAndHoldEnabled;
}

- (JWMMainView*)initWithWindow:(jwm::WindowMac*)initWindow;

- (BOOL)isPressAndHoldEnabled;
- (void)setPressAndHoldEnabled:(BOOL)enabled;

@end
