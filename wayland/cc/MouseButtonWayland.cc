#include "MouseButtonWayland.hh"


jwm::MouseButton jwm::MouseButtonWayland::fromNative(uint32_t v) {
    switch (v) {
        case 0x110: return jwm::MouseButton::PRIMARY; 
        case 0x112: return jwm::MouseButton::MIDDLE;
        case 0x111: return jwm::MouseButton::SECONDARY;
        case 0x116: return jwm::MouseButton::BACK;
        case 0x115: return jwm::MouseButton::FORWARD;
    }
    return jwm::MouseButton::PRIMARY;
}

bool jwm::MouseButtonWayland::isButton(uint32_t v) {
    return v >= 0x110 && v <= 0x116; // mouse wheel buttons
}

int jwm::MouseButtonWayland::fromNativeMask(unsigned v) {
    int res = 0;
    if (v & 0x100) res |= int(jwm::MouseButton::PRIMARY);
    if (v & 0x400) res |= int(jwm::MouseButton::SECONDARY);
    if (v & 0x200) res |= int(jwm::MouseButton::MIDDLE);
    return res;
}
