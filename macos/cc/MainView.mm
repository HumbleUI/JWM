#include <algorithm>
#include <Carbon/Carbon.h>
#include <jni.h>
#include "impl/JNILocal.hh"
#include "impl/Library.hh"
#include "WindowMac.hh"
#include "MainView.hh"

namespace jwm {
Key kKeyTable[128];
NSEventModifierFlags kLastFlags = 0;

void initKeyTable() {
    std::fill(kKeyTable, kKeyTable + 128, Key::UNDEFINED);

    kKeyTable[kVK_ANSI_A] = Key::A;
    kKeyTable[kVK_ANSI_S] = Key::S;
    kKeyTable[kVK_ANSI_D] = Key::D;
    kKeyTable[kVK_ANSI_F] = Key::F;
    kKeyTable[kVK_ANSI_H] = Key::H;
    kKeyTable[kVK_ANSI_G] = Key::G;
    kKeyTable[kVK_ANSI_Z] = Key::Z;
    kKeyTable[kVK_ANSI_X] = Key::X;
    kKeyTable[kVK_ANSI_C] = Key::C;
    kKeyTable[kVK_ANSI_V] = Key::V;
    kKeyTable[kVK_ANSI_B] = Key::B;
    kKeyTable[kVK_ANSI_Q] = Key::Q;
    kKeyTable[kVK_ANSI_W] = Key::W;
    kKeyTable[kVK_ANSI_E] = Key::E;
    kKeyTable[kVK_ANSI_R] = Key::R;
    kKeyTable[kVK_ANSI_Y] = Key::Y;
    kKeyTable[kVK_ANSI_T] = Key::T;
    kKeyTable[kVK_ANSI_1] = Key::DIGIT1;
    kKeyTable[kVK_ANSI_2] = Key::DIGIT2;
    kKeyTable[kVK_ANSI_3] = Key::DIGIT3;
    kKeyTable[kVK_ANSI_4] = Key::DIGIT4;
    kKeyTable[kVK_ANSI_6] = Key::DIGIT6;
    kKeyTable[kVK_ANSI_5] = Key::DIGIT5;
    kKeyTable[kVK_ANSI_Equal] = Key::EQUALS;
    kKeyTable[kVK_ANSI_9] = Key::DIGIT9;
    kKeyTable[kVK_ANSI_7] = Key::DIGIT7;
    kKeyTable[kVK_ANSI_Minus] = Key::MINUS;
    kKeyTable[kVK_ANSI_8] = Key::DIGIT8;
    kKeyTable[kVK_ANSI_0] = Key::DIGIT0;
    kKeyTable[kVK_ANSI_RightBracket] = Key::CLOSE_BRACKET;
    kKeyTable[kVK_ANSI_O] = Key::O;
    kKeyTable[kVK_ANSI_U] = Key::U;
    kKeyTable[kVK_ANSI_LeftBracket] = Key::OPEN_BRACKET;
    kKeyTable[kVK_ANSI_I] = Key::I;
    kKeyTable[kVK_ANSI_P] = Key::P;
    kKeyTable[kVK_ANSI_L] = Key::L;
    kKeyTable[kVK_ANSI_J] = Key::J;
    kKeyTable[kVK_ANSI_Quote] = Key::QUOTE;
    kKeyTable[kVK_ANSI_K] = Key::K;
    kKeyTable[kVK_ANSI_Semicolon] = Key::SEMICOLON;
    kKeyTable[kVK_ANSI_Backslash] = Key::BACK_SLASH;
    kKeyTable[kVK_ANSI_Comma] = Key::COMMA;
    kKeyTable[kVK_ANSI_Slash] = Key::SLASH;
    kKeyTable[kVK_ANSI_N] = Key::N;
    kKeyTable[kVK_ANSI_M] = Key::M;
    kKeyTable[kVK_ANSI_Period] = Key::PERIOD;
    kKeyTable[kVK_ANSI_Grave] = Key::BACK_QUOTE;
    kKeyTable[kVK_ANSI_KeypadDecimal] = Key::DECIMAL;
    kKeyTable[kVK_ANSI_KeypadMultiply] = Key::MULTIPLY;
    kKeyTable[kVK_ANSI_KeypadPlus] = Key::ADD;
    kKeyTable[kVK_ANSI_KeypadClear] = Key::CLEAR; // ?
    kKeyTable[kVK_ANSI_KeypadDivide] = Key::DIVIDE;
    kKeyTable[kVK_ANSI_KeypadEnter] = Key::ENTER; // ?
    kKeyTable[kVK_ANSI_KeypadMinus] = Key::SUBTRACT;
    kKeyTable[kVK_ANSI_KeypadEquals] = Key::EQUALS; // ?
    kKeyTable[kVK_ANSI_Keypad0] = Key::NUMPAD0;
    kKeyTable[kVK_ANSI_Keypad1] = Key::NUMPAD1;
    kKeyTable[kVK_ANSI_Keypad2] = Key::NUMPAD2;
    kKeyTable[kVK_ANSI_Keypad3] = Key::NUMPAD3;
    kKeyTable[kVK_ANSI_Keypad4] = Key::NUMPAD4;
    kKeyTable[kVK_ANSI_Keypad5] = Key::NUMPAD5;
    kKeyTable[kVK_ANSI_Keypad6] = Key::NUMPAD6;
    kKeyTable[kVK_ANSI_Keypad7] = Key::NUMPAD7;
    kKeyTable[kVK_ANSI_Keypad8] = Key::NUMPAD8;
    kKeyTable[kVK_ANSI_Keypad9] = Key::NUMPAD9;

    /* keycodes for keys that are independent of keyboard layout*/
    kKeyTable[kVK_Return] = Key::ENTER;
    kKeyTable[kVK_Tab] = Key::TAB;
    kKeyTable[kVK_Space] = Key::SPACE;
    kKeyTable[kVK_Delete] = Key::BACK_SPACE;
    kKeyTable[kVK_Escape] = Key::ESCAPE;
    kKeyTable[kVK_Command] = Key::META; // ?
    kKeyTable[kVK_Shift] = Key::SHIFT;
    kKeyTable[kVK_CapsLock] = Key::CAPS;
    kKeyTable[kVK_Option] = Key::ALT;
    kKeyTable[kVK_Control] = Key::CONTROL;
    kKeyTable[kVK_RightCommand] = Key::META; // ?
    kKeyTable[kVK_RightShift] = Key::SHIFT; // ?
    kKeyTable[kVK_RightOption] = Key::ALT; // ?
    kKeyTable[kVK_RightControl] = Key::CONTROL; // ?
    // kKeyTable[kVK_Function] = Key::;
    kKeyTable[kVK_F17] = Key::F17;
    kKeyTable[kVK_VolumeUp] = Key::VOLUME_UP;
    kKeyTable[kVK_VolumeDown] = Key::VOLUME_DOWN;
    kKeyTable[kVK_Mute] = Key::MUTE;
    kKeyTable[kVK_F18] = Key::F18;
    kKeyTable[kVK_F19] = Key::F19;
    kKeyTable[kVK_F20] = Key::F20;
    kKeyTable[kVK_F5] = Key::F5;
    kKeyTable[kVK_F6] = Key::F6;
    kKeyTable[kVK_F7] = Key::F7;
    kKeyTable[kVK_F3] = Key::F3;
    kKeyTable[kVK_F8] = Key::F8;
    kKeyTable[kVK_F9] = Key::F9;
    kKeyTable[kVK_F11] = Key::F11;
    kKeyTable[kVK_F13] = Key::F13;
    kKeyTable[kVK_F16] = Key::F16;
    kKeyTable[kVK_F14] = Key::F14;
    kKeyTable[kVK_F10] = Key::F10;
    kKeyTable[kVK_F12] = Key::F12;
    kKeyTable[kVK_F15] = Key::F15;
    kKeyTable[kVK_Help] = Key::HELP;
    kKeyTable[kVK_Home] = Key::HOME;
    kKeyTable[kVK_PageUp] = Key::PAGE_UP;
    kKeyTable[kVK_ForwardDelete] = Key::DEL;
    kKeyTable[kVK_F4] = Key::F4;
    kKeyTable[kVK_End] = Key::END;
    kKeyTable[kVK_F2] = Key::F2;
    kKeyTable[kVK_PageDown] = Key::PAGE_DOWN;
    kKeyTable[kVK_F1] = Key::F1;
    kKeyTable[kVK_LeftArrow] = Key::LEFT;
    kKeyTable[kVK_RightArrow] = Key::RIGHT;
    kKeyTable[kVK_DownArrow] = Key::DOWN;
    kKeyTable[kVK_UpArrow] = Key::UP;

    /* ISO keyboards only*/
    // kKeyTable[kVK_ISO_Section] = Key::;

    /* JIS keyboards only*/
    // kKeyTable[kVK_JIS_Yen] = Key::;
    // kKeyTable[kVK_JIS_Underscore] = Key::;
    // kKeyTable[kVK_JIS_KeypadComma] = Key::;
    // kKeyTable[kVK_JIS_Eisu] = Key::;
    kKeyTable[kVK_JIS_Kana] = Key::KANA;
}
} // namespace jwm

@implementation MainView {
    jwm::WindowMac* fWindow;
    // A TrackingArea prevents us from capturing events outside the view
    NSTrackingArea* fTrackingArea;
    // We keep track of the state of the modifier keys on each event in order to synthesize
    // key-up/down events for each modifier.
    jwm::ModifierKey fLastModifiers;
}

- (MainView*)initWithWindow:(jwm::WindowMac*)initWindow {
    self = [super init];

    fWindow = initWindow;
    fTrackingArea = nil;

    [self updateTrackingAreas];

    return self;
}

- (void)dealloc {
    [fTrackingArea release];
    [super dealloc];
}

- (BOOL)isOpaque {
    return YES;
}

- (BOOL)canBecomeKeyView {
    return YES;
}

- (BOOL)acceptsFirstResponder {
    return YES;
}

- (void)updateTrackingAreas {
    if (fTrackingArea != nil) {
        [self removeTrackingArea:fTrackingArea];
        [fTrackingArea release];
    }

    const NSTrackingAreaOptions options = NSTrackingMouseEnteredAndExited |
                                          NSTrackingActiveInKeyWindow |
                                          NSTrackingEnabledDuringMouseDrag |
                                          NSTrackingCursorUpdate |
                                          NSTrackingInVisibleRect |
                                          NSTrackingAssumeInside;

    fTrackingArea = [[NSTrackingArea alloc] initWithRect:[self bounds]
                                                 options:options
                                                   owner:self
                                                userInfo:nil];

    [self addTrackingArea:fTrackingArea];
    [super updateTrackingAreas];
}

- (void)mouseMoved:(NSEvent *)event {
    NSView* view = fWindow->fNSWindow.contentView;
    CGFloat scale = fWindow->getScale();

    // skui::ModifierKey modifiers = [self updateModifierKeys:event];

    const NSPoint pos = [event locationInWindow];
    const NSRect rect = [view frame];
    jwm::JNILocal<jobject> eventObj(fWindow->fEnv, jwm::classes::EventMouseMove::make(fWindow->fEnv, (jint) (pos.x * scale), (jint) ((rect.size.height - pos.y) * scale)));
    fWindow->dispatch(eventObj.get());
}

- (void)keyDown:(NSEvent *)event {
    unsigned short keyCode = [event keyCode];
    jwm::Key key = keyCode < 128 ? jwm::kKeyTable[keyCode] : jwm::Key::UNDEFINED;
    jwm::JNILocal<jobject> eventObj(fWindow->fEnv, jwm::classes::EventKeyboard::make(fWindow->fEnv, static_cast<int>(key), (jboolean) true));
    fWindow->dispatch(eventObj.get());
}

- (void)keyUp:(NSEvent *)event {
    unsigned short keyCode = [event keyCode];
    jwm::Key key = keyCode < 128 ? jwm::kKeyTable[keyCode] : jwm::Key::UNDEFINED;
    jwm::JNILocal<jobject> eventObj(fWindow->fEnv, jwm::classes::EventKeyboard::make(fWindow->fEnv, static_cast<int>(key), (jboolean) false));
    fWindow->dispatch(eventObj.get());
}

- (void)flagsChanged:(NSEvent *)event {
    NSEventModifierFlags flags = [event modifierFlags];

    if ((jwm::kLastFlags & NSEventModifierFlagShift) == 0 && (flags & NSEventModifierFlagShift) != 0) {
        jwm::JNILocal<jobject> eventObj(fWindow->fEnv, jwm::classes::EventKeyboard::make(fWindow->fEnv, static_cast<int>(jwm::Key::SHIFT), (jboolean) true));
        fWindow->dispatch(eventObj.get());
    }

    if ((jwm::kLastFlags & NSEventModifierFlagShift) != 0 && (flags & NSEventModifierFlagShift) == 0) {
        jwm::JNILocal<jobject> eventObj(fWindow->fEnv, jwm::classes::EventKeyboard::make(fWindow->fEnv, static_cast<int>(jwm::Key::SHIFT), (jboolean) false));
        fWindow->dispatch(eventObj.get());
    }

    if ((jwm::kLastFlags & NSEventModifierFlagControl) == 0 && (flags & NSEventModifierFlagControl) != 0) {
        jwm::JNILocal<jobject> eventObj(fWindow->fEnv, jwm::classes::EventKeyboard::make(fWindow->fEnv, static_cast<int>(jwm::Key::CONTROL), (jboolean) true));
        fWindow->dispatch(eventObj.get());
    }

    if ((jwm::kLastFlags & NSEventModifierFlagControl) != 0 && (flags & NSEventModifierFlagControl) == 0) {
        jwm::JNILocal<jobject> eventObj(fWindow->fEnv, jwm::classes::EventKeyboard::make(fWindow->fEnv, static_cast<int>(jwm::Key::CONTROL), (jboolean) false));
        fWindow->dispatch(eventObj.get());
    }

    if ((jwm::kLastFlags & NSEventModifierFlagOption) == 0 && (flags & NSEventModifierFlagOption) != 0) {
        jwm::JNILocal<jobject> eventObj(fWindow->fEnv, jwm::classes::EventKeyboard::make(fWindow->fEnv, static_cast<int>(jwm::Key::ALT), (jboolean) true));
        fWindow->dispatch(eventObj.get());
    }

    if ((jwm::kLastFlags & NSEventModifierFlagOption) != 0 && (flags & NSEventModifierFlagOption) == 0) {
        jwm::JNILocal<jobject> eventObj(fWindow->fEnv, jwm::classes::EventKeyboard::make(fWindow->fEnv, static_cast<int>(jwm::Key::ALT), (jboolean) false));
        fWindow->dispatch(eventObj.get());
    }

    if ((jwm::kLastFlags & NSEventModifierFlagCommand) == 0 && (flags & NSEventModifierFlagCommand) != 0) {
        jwm::JNILocal<jobject> eventObj(fWindow->fEnv, jwm::classes::EventKeyboard::make(fWindow->fEnv, static_cast<int>(jwm::Key::COMMAND), (jboolean) true));
        fWindow->dispatch(eventObj.get());
    }

    if ((jwm::kLastFlags & NSEventModifierFlagCommand) != 0 && (flags & NSEventModifierFlagCommand) == 0) {
        jwm::JNILocal<jobject> eventObj(fWindow->fEnv, jwm::classes::EventKeyboard::make(fWindow->fEnv, static_cast<int>(jwm::Key::COMMAND), (jboolean) false));
        fWindow->dispatch(eventObj.get());
    }

    if ((jwm::kLastFlags & NSEventModifierFlagFunction) == 0 && (flags & NSEventModifierFlagFunction) != 0) {
        jwm::JNILocal<jobject> eventObj(fWindow->fEnv, jwm::classes::EventKeyboard::make(fWindow->fEnv, static_cast<int>(jwm::Key::FUNCTION), (jboolean) true));
        fWindow->dispatch(eventObj.get());
    }

    if ((jwm::kLastFlags & NSEventModifierFlagFunction) != 0 && (flags & NSEventModifierFlagFunction) == 0) {
        jwm::JNILocal<jobject> eventObj(fWindow->fEnv, jwm::classes::EventKeyboard::make(fWindow->fEnv, static_cast<int>(jwm::Key::FUNCTION), (jboolean) false));
        fWindow->dispatch(eventObj.get());
    }

    if ((jwm::kLastFlags & NSEventModifierFlagCapsLock) == 0 && (flags & NSEventModifierFlagCapsLock) != 0) {
        jwm::JNILocal<jobject> eventObj(fWindow->fEnv, jwm::classes::EventKeyboard::make(fWindow->fEnv, static_cast<int>(jwm::Key::CAPS), (jboolean) true));
        fWindow->dispatch(eventObj.get());
    }

    if ((jwm::kLastFlags & NSEventModifierFlagCapsLock) != 0 && (flags & NSEventModifierFlagCapsLock) == 0) {
        jwm::JNILocal<jobject> eventObj(fWindow->fEnv, jwm::classes::EventKeyboard::make(fWindow->fEnv, static_cast<int>(jwm::Key::CAPS), (jboolean) false));
        fWindow->dispatch(eventObj.get());
    }

    jwm::kLastFlags = flags;
}

@end
