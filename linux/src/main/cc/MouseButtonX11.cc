#include "MouseButtonX11.hh"


jwm::MouseButton jwm::MouseButtonX11::fromNative(uint32_t v) {
    switch (v) {
        case 1: return jwm::MouseButton::PRIMARY; 
        case 2: return jwm::MouseButton::MIDDLE;
        case 3: return jwm::MouseButton::SECONDARY;
        case 4: return jwm::MouseButton::BACK;
        case 5: return jwm::MouseButton::FORWARD;
    }
    return jwm::MouseButton::PRIMARY;
}

int jwm::MouseButtonX11::fromNativeMask(unsigned v) {
    int res = 0;
    if (v & 0x100) res |= int(jwm::MouseButton::PRIMARY);
    if (v & 0x400) res |= int(jwm::MouseButton::SECONDARY);
    if (v & 0x200) res |= int(jwm::MouseButton::MIDDLE);
    return res;
}