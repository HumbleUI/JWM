#pragma once

namespace jwm {
    // must be kept in sync with ZOrder.java
    enum class ZOrder {
        NORMAL,
        FLOATING,
        MODAL_PANEL,
        MAIN_MENU,
        STATUS,
        POP_UP_MENU,
        SCREEN_SAVER,
    };
}
