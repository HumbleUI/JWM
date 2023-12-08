#include "KeyWayland.hh"
#include "KeyModifier.hh"
#include <xkbcommon/xkbcommon.h>
#include <xkbcommon/xkbcommon-names.h>

int jwm::KeyWayland::getModifiers(xkb_state* state) {

    int m = 0;
    
    if (!state)
        return 0;
    if (xkb_state_mod_name_is_active(state, XKB_MOD_NAME_SHIFT, XKB_STATE_MODS_EFFECTIVE)) m |= (int)jwm::KeyModifier::SHIFT;
    if (xkb_state_mod_name_is_active(state, XKB_MOD_NAME_CTRL , XKB_STATE_MODS_EFFECTIVE)) m |= (int)jwm::KeyModifier::CONTROL;
    if (xkb_state_mod_name_is_active(state, XKB_MOD_NAME_ALT  , XKB_STATE_MODS_EFFECTIVE)) m |= (int)jwm::KeyModifier::ALT;
    if (xkb_state_mod_name_is_active(state, XKB_MOD_NAME_LOGO , XKB_STATE_MODS_EFFECTIVE)) m |= (int)jwm::KeyModifier::LINUX_META;
    if (xkb_state_mod_name_is_active(state, "Super"           , XKB_STATE_MODS_EFFECTIVE)) m |= (int)jwm::KeyModifier::LINUX_SUPER;

    return m;
}
/*
int jwm::KeyWayland::getModifiersFromMask(int mask) {
    int m = getModifiers();
    // ???
    // if (mask & ShiftMask  ) m |= (int)jwm::KeyModifier::SHIFT;
    // if (mask & ControlMask) m |= (int)jwm::KeyModifier::CONTROL;
    // if (mask & Mod1Mask   ) m |= (int)jwm::KeyModifier::ALT;

    return m;
}*/

jwm::Key jwm::KeyWayland::fromNative(uint32_t v) { 
    switch (v) {
        // Modifiers
        case XKB_KEY_Caps_Lock: return Key::CAPS_LOCK;
        case XKB_KEY_Shift_R:
        case XKB_KEY_Shift_L: return Key::SHIFT;
        case XKB_KEY_Control_R:
        case XKB_KEY_Control_L: return Key::CONTROL;
        case XKB_KEY_Alt_R:
        case XKB_KEY_Alt_L: return Key::ALT;
        // Key::WIN_LOGO
        case XKB_KEY_Super_L:
        case XKB_KEY_Super_R: return Key::LINUX_SUPER;
        case XKB_KEY_Meta_L:
        case XKB_KEY_Meta_R: return Key::LINUX_META;
        // Key::MAC_COMMAND
        // Key::MAC_OPTION
        // Key::MAC_FN

        // Rest of the keys
        case XKB_KEY_Return: return Key::ENTER;
        case XKB_KEY_BackSpace: return Key::BACKSPACE;
        case XKB_KEY_Tab: return Key::TAB;
        case XKB_KEY_Cancel: return Key::CANCEL;
        case XKB_KEY_Clear: return Key::CLEAR;
        case XKB_KEY_Pause: return Key::PAUSE;
        case XKB_KEY_Escape: return Key::ESCAPE;
        case XKB_KEY_space: return Key::SPACE;
        case XKB_KEY_Page_Up: return Key::PAGE_UP;
        case XKB_KEY_Page_Down: return Key::PAGE_DOWN;
        case XKB_KEY_End: return Key::END;
        case XKB_KEY_Home: return Key::HOME;
        case XKB_KEY_Left: return Key::LEFT;
        case XKB_KEY_Up: return Key::UP;
        case XKB_KEY_Right: return Key::RIGHT;
        case XKB_KEY_Down: return Key::DOWN;
        case XKB_KEY_comma: return Key::COMMA;
        case XKB_KEY_minus: return Key::MINUS;
        case XKB_KEY_period: return Key::PERIOD;
        case XKB_KEY_slash: return Key::SLASH;
        case XKB_KEY_0: return Key::DIGIT0;
        case XKB_KEY_1: return Key::DIGIT1;
        case XKB_KEY_2: return Key::DIGIT2;
        case XKB_KEY_3: return Key::DIGIT3;
        case XKB_KEY_4: return Key::DIGIT4;
        case XKB_KEY_5: return Key::DIGIT5;
        case XKB_KEY_6: return Key::DIGIT6;
        case XKB_KEY_7: return Key::DIGIT7;
        case XKB_KEY_8: return Key::DIGIT8;
        case XKB_KEY_9: return Key::DIGIT9;
        case XKB_KEY_semicolon: return Key::SEMICOLON;
        case XKB_KEY_equal: return Key::EQUALS;
        case XKB_KEY_a: return Key::A;
        case XKB_KEY_b: return Key::B;
        case XKB_KEY_c: return Key::C;
        case XKB_KEY_d: return Key::D;
        case XKB_KEY_e: return Key::E;
        case XKB_KEY_f: return Key::F;
        case XKB_KEY_g: return Key::G;
        case XKB_KEY_h: return Key::H;
        case XKB_KEY_i: return Key::I;
        case XKB_KEY_j: return Key::J;
        case XKB_KEY_k: return Key::K;
        case XKB_KEY_l: return Key::L;
        case XKB_KEY_m: return Key::M;
        case XKB_KEY_n: return Key::N;
        case XKB_KEY_o: return Key::O;
        case XKB_KEY_p: return Key::P;
        case XKB_KEY_q: return Key::Q;
        case XKB_KEY_r: return Key::R;
        case XKB_KEY_s: return Key::S;
        case XKB_KEY_t: return Key::T;
        case XKB_KEY_u: return Key::U;
        case XKB_KEY_v: return Key::V;
        case XKB_KEY_w: return Key::W;
        case XKB_KEY_x: return Key::X;
        case XKB_KEY_y: return Key::Y;
        case XKB_KEY_z: return Key::Z;
        case XKB_KEY_bracketleft: return Key::OPEN_BRACKET;
        case XKB_KEY_backslash: return Key::BACK_SLASH;
        case XKB_KEY_bracketright: return Key::CLOSE_BRACKET;
        case XKB_KEY_KP_0: return Key::DIGIT0;
        case XKB_KEY_KP_1: return Key::DIGIT1;
        case XKB_KEY_KP_2: return Key::DIGIT2;
        case XKB_KEY_KP_3: return Key::DIGIT3;
        case XKB_KEY_KP_4: return Key::DIGIT4;
        case XKB_KEY_KP_5: return Key::DIGIT5;
        case XKB_KEY_KP_6: return Key::DIGIT6;
        case XKB_KEY_KP_7: return Key::DIGIT7;
        case XKB_KEY_KP_8: return Key::DIGIT8;
        case XKB_KEY_KP_9: return Key::DIGIT9;
        case XKB_KEY_multiply: return Key::MULTIPLY;
        case XKB_KEY_KP_Add: return Key::ADD;
        case XKB_KEY_KP_Separator: return Key::SEPARATOR;
        case XKB_KEY_KP_Subtract: return Key::MINUS;
        case XKB_KEY_KP_Decimal: return Key::PERIOD;
        case XKB_KEY_KP_Divide: return Key::SLASH;
        case XKB_KEY_KP_Delete: return Key::DEL;
        case XKB_KEY_Delete: return Key::DEL;
        case XKB_KEY_Num_Lock: return Key::NUM_LOCK;
        case XKB_KEY_Scroll_Lock: return Key::SCROLL_LOCK;
        case XKB_KEY_F1: return Key::F1;
        case XKB_KEY_F2: return Key::F2;
        case XKB_KEY_F3: return Key::F3;
        case XKB_KEY_F4: return Key::F4;
        case XKB_KEY_F5: return Key::F5;
        case XKB_KEY_F6: return Key::F6;
        case XKB_KEY_F7: return Key::F7;
        case XKB_KEY_F8: return Key::F8;
        case XKB_KEY_F9: return Key::F9;
        case XKB_KEY_F10: return Key::F10;
        case XKB_KEY_F11: return Key::F11;
        case XKB_KEY_F12: return Key::F12;
        case XKB_KEY_F13: return Key::F13;
        case XKB_KEY_F14: return Key::F14;
        case XKB_KEY_F15: return Key::F15;
        case XKB_KEY_F16: return Key::F16;
        case XKB_KEY_F17: return Key::F17;
        case XKB_KEY_F18: return Key::F18;
        case XKB_KEY_F19: return Key::F19;
        case XKB_KEY_F20: return Key::F20;
        case XKB_KEY_F21: return Key::F21;
        case XKB_KEY_F22: return Key::F22;
        case XKB_KEY_F23: return Key::F23;
        case XKB_KEY_F24: return Key::F24;
        case XKB_KEY_Print: return Key::PRINTSCREEN;
        case XKB_KEY_Insert: return Key::INSERT;
        case XKB_KEY_Help: return Key::HELP;
        case XKB_KEY_grave: return Key::BACK_QUOTE;
        case XKB_KEY_quoteright: return Key::QUOTE;
        case XKB_KEY_Menu: return Key::MENU;
        // Key::KANA
        // Key::VOLUME_UP
        // Key::VOLUME_DOWN
        // Key::MUTE
        default: return Key::UNDEFINED;
    } 
}
