#pragma once

namespace jwm {
    // must be kept in sync with Key.java
    enum class Key {
        UNDEFINED,

        // KeyModifiers
        CAPS_LOCK,
        SHIFT,
        CONTROL,
        ALT,
        WIN_LOGO,
        LINUX_META,
        LINUX_SUPER,
        MAC_COMMAND,
        MAC_OPTION,
        MAC_FN,
        
        // Rest of the keys
        ENTER,
        BACKSPACE,
        TAB,
        CANCEL,
        CLEAR,
        PAUSE,
        ESCAPE,
        SPACE,
        PAGE_UP,
        PAGE_DOWN,
        END,
        HOME,
        LEFT,
        UP,
        RIGHT,
        DOWN,
        COMMA,
        MINUS,
        PERIOD,
        SLASH,
        DIGIT0,
        DIGIT1,
        DIGIT2,
        DIGIT3,
        DIGIT4,
        DIGIT5,
        DIGIT6,
        DIGIT7,
        DIGIT8,
        DIGIT9,
        SEMICOLON,
        EQUALS,
        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,
        OPEN_BRACKET,
        BACK_SLASH,
        CLOSE_BRACKET,
        MULTIPLY,
        ADD,
        SEPARATOR,
        DEL,
        NUM_LOCK,
        SCROLL_LOCK,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        F13,
        F14,
        F15,
        F16,
        F17,
        F18,
        F19,
        F20,
        F21,
        F22,
        F23,
        F24,
        PRINTSCREEN,
        INSERT,
        HELP,
        BACK_QUOTE,
        QUOTE,
        MENU,
        KANA,
        VOLUME_UP,
        VOLUME_DOWN,
        MUTE,

        _KEY_COUNT, // must be the last
    };
}