#pragma once

#include <cstdint>
#include "Key.hh"

namespace jwm {
    namespace KeyX11 {
        jwm::Key fromNative(uint32_t v);
    }
}