#include "KeyX11.hh"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>

jwm::Key jwm::KeyX11::fromNative(uint32_t v) {
    switch (v) {
        case XK_Return: return Key::ENTER;
        case XK_BackSpace: return Key::BACK_SPACE;
        case XK_Tab: return Key::TAB;
        case XK_Cancel: return Key::CANCEL;
        case XK_Clear: return Key::CLEAR;
        case XK_Shift_R:
        case XK_Shift_L: return Key::SHIFT;
        case XK_Control_R:
        case XK_Control_L: return Key::CONTROL;
        case XK_Alt_R:
        case XK_Alt_L: return Key::ALT;
        case XK_Pause: return Key::PAUSE;
        case XK_Caps_Lock: return Key::CAPS;
        case XK_Escape: return Key::ESCAPE;
        case XK_space: return Key::SPACE;
        case XK_Page_Up: return Key::PAGE_UP;
        case XK_Page_Down: return Key::PAGE_DOWN;
        case XK_End: return Key::END;
        case XK_Home: return Key::HOME;
        case XK_Left: return Key::LEFT;
        case XK_Up: return Key::UP;
        case XK_Right: return Key::RIGHT;
        case XK_Down: return Key::DOWN;
        case XK_comma: return Key::COMMA;
        case XK_minus: return Key::MINUS;
        case XK_period: return Key::PERIOD;
        case XK_slash: return Key::SLASH;
        case XK_0: return Key::DIGIT0;
        case XK_1: return Key::DIGIT1;
        case XK_2: return Key::DIGIT2;
        case XK_3: return Key::DIGIT3;
        case XK_4: return Key::DIGIT4;
        case XK_5: return Key::DIGIT5;
        case XK_6: return Key::DIGIT6;
        case XK_7: return Key::DIGIT7;
        case XK_8: return Key::DIGIT8;
        case XK_9: return Key::DIGIT9;
        case XK_semicolon: return Key::SEMICOLON;
        case XK_equal: return Key::EQUALS;
        case XK_a: return Key::A;
        case XK_b: return Key::B;
        case XK_c: return Key::C;
        case XK_d: return Key::D;
        case XK_e: return Key::E;
        case XK_f: return Key::F;
        case XK_g: return Key::G;
        case XK_h: return Key::H;
        case XK_i: return Key::I;
        case XK_j: return Key::J;
        case XK_k: return Key::K;
        case XK_l: return Key::L;
        case XK_m: return Key::M;
        case XK_n: return Key::N;
        case XK_o: return Key::O;
        case XK_p: return Key::P;
        case XK_q: return Key::Q;
        case XK_r: return Key::R;
        case XK_s: return Key::S;
        case XK_t: return Key::T;
        case XK_u: return Key::U;
        case XK_v: return Key::V;
        case XK_w: return Key::W;
        case XK_x: return Key::X;
        case XK_y: return Key::Y;
        case XK_z: return Key::Z;
        case XK_bracketleft: return Key::OPEN_BRACKET;
        case XK_backslash: return Key::BACK_SLASH;
        case XK_bracketright: return Key::CLOSE_BRACKET;
        case XK_KP_0: return Key::NUMPAD0;
        case XK_KP_1: return Key::NUMPAD1;
        case XK_KP_2: return Key::NUMPAD2;
        case XK_KP_3: return Key::NUMPAD3;
        case XK_KP_4: return Key::NUMPAD4;
        case XK_KP_5: return Key::NUMPAD5;
        case XK_KP_6: return Key::NUMPAD6;
        case XK_KP_7: return Key::NUMPAD7;
        case XK_KP_8: return Key::NUMPAD8;
        case XK_KP_9: return Key::NUMPAD9;
        case XK_multiply: return Key::MULTIPLY;
        case XK_KP_Add: return Key::ADD;
        case XK_KP_Separator: return Key::SEPARATOR;
        case XK_KP_Subtract: return Key::SUBTRACT;
        case XK_KP_Decimal: return Key::DECIMAL;
        case XK_KP_Divide: return Key::DIVIDE;
        case XK_KP_Delete: return Key::DEL;
        case XK_Delete:
        case XK_Num_Lock: return Key::NUM_LOCK;
        case XK_Scroll_Lock: return Key::SCROLL_LOCK;
        case XK_F1: return Key::F1;
        case XK_F2: return Key::F2;
        case XK_F3: return Key::F3;
        case XK_F4: return Key::F4;
        case XK_F5: return Key::F5;
        case XK_F6: return Key::F6;
        case XK_F7: return Key::F7;
        case XK_F8: return Key::F8;
        case XK_F9: return Key::F9;
        case XK_F10: return Key::F10;
        case XK_F11: return Key::F11;
        case XK_F12: return Key::F12;
        case XK_F13: return Key::F13;
        case XK_F14: return Key::F14;
        case XK_F15: return Key::F15;
        case XK_F16: return Key::F16;
        case XK_F17: return Key::F17;
        case XK_F18: return Key::F18;
        case XK_F19: return Key::F19;
        case XK_F20: return Key::F20;
        case XK_F21: return Key::F21;
        case XK_F22: return Key::F22;
        case XK_F23: return Key::F23;
        case XK_F24: return Key::F24;
        case XK_Print: return Key::PRINTSCREEN;
        case XK_Insert: return Key::INSERT;
        case XK_Help: return Key::HELP;
        case XK_Meta_R:
        case XK_Meta_L: return Key::META;
        case XK_grave: return Key::BACK_QUOTE;
        case XK_quoteright: return Key::QUOTE;
        case XK_KP_Up: return Key::KP_UP;
        case XK_KP_Down: return Key::KP_DOWN;
        case XK_KP_Left: return Key::KP_LEFT;
        case XK_KP_Right: return Key::KP_RIGHT;
        case XK_dead_grave: return Key::DEAD_GRAVE;
        case XK_dead_acute: return Key::DEAD_ACUTE;
        case XK_dead_circumflex: return Key::DEAD_CIRCUMFLEX;
        case XK_dead_tilde: return Key::DEAD_TILDE;
        case XK_dead_macron: return Key::DEAD_MACRON;
        case XK_dead_breve: return Key::DEAD_BREVE;
        case XK_dead_abovedot: return Key::DEAD_ABOVEDOT;
        case XK_dead_diaeresis: return Key::DEAD_DIAERESIS;
        case XK_dead_abovering: return Key::DEAD_ABOVERING;
        case XK_dead_doubleacute: return Key::DEAD_DOUBLEACUTE;
        case XK_dead_caron: return Key::DEAD_CARON;
        case XK_dead_cedilla: return Key::DEAD_CEDILLA;
        case XK_dead_ogonek: return Key::DEAD_OGONEK;
        case XK_dead_iota: return Key::DEAD_IOTA;
        case XK_dead_voiced_sound: return Key::DEAD_VOICED_SOUND;
        case XK_dead_semivoiced_sound: return Key::DEAD_SEMIVOICED_SOUND;
        case XK_ampersand: return Key::AMPERSAND;
        case XK_asterisk: return Key::ASTERISK;
        case XK_quotedbl: return Key::QUOTEDBL;
        case XK_less: return Key::LESS;
        case XK_greater: return Key::GREATER;
        case XK_braceleft: return Key::BRACELEFT;
        case XK_braceright: return Key::BRACERIGHT;

        case XK_at: return Key::AT;
        case XK_colon: return Key::COLON;
        case XK_ucircumflex: return Key::CIRCUMFLEX;
        case XK_dollar: return Key::DOLLAR;
        case XK_EuroSign: return Key::EURO_SIGN;
        case XK_exclam: return Key::EXCLAMATION_MARK;
        case XK_exclamdown: return Key::INVERTED_EXCLAMATION_MARK;
        case XK_parenleft: return Key::LEFT_PARENTHESIS;
        case XK_numbersign: return Key::NUMBER_SIGN;
        case XK_plus: return Key::PLUS;
        case XK_parenright: return Key::RIGHT_PARENTHESIS;
        case XK_underscore: return Key::UNDERSCORE;
        //case XK_Meta_L: return Key::WINDOWS;
        case XK_Menu: return Key::CONTEXT_MENU;

        default: return Key::UNDEFINED;
    }
}