#pragma once
#import <Cocoa/Cocoa.h>
#include "WindowMac.hh"
#include "Key.hh"

namespace jwm {
    extern Key kKeyTable[];
    extern BOOL kPressAndHoldEnabled;
    void initKeyTable();
}

@interface JWMMainView : NSView <NSTextInputClient> {
    BOOL fInPressAndHold;
    BOOL fKeyEventsNeeded;
}

- (JWMMainView*)initWithWindow:(jwm::WindowMac*)initWindow;

@end
