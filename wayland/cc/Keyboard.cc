#include "Keyboard.hh"
#include "WindowManagerWayland.hh"
#include "WindowWayland.hh"
#include "KeyWayland.hh"
#include <unistd.h>
#include <impl/JNILocal.hh>
#include "StringUTF16.hh"
#include <system_error>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>
#include "AppWayland.hh"
#include <chrono>
using namespace jwm;

// I've noticed that pointers to lambdas are null for some reason. 
// No idea what's wrong. Going to cry myself to sleep tonight.
static void kbKeymap(void* data, wl_keyboard* kb, uint32_t format, int32_t fd, uint32_t size) {
    auto self = reinterpret_cast<Keyboard*>(data);

    if (format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1) {
        close(fd);
        fprintf(stderr, "no xkb keymap\n");
        return;
    }

    char* map_str = reinterpret_cast<char*>(mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0));
    if (map_str == MAP_FAILED) {
        close(fd);
        fprintf(stderr, "keymap mmap failed: %s", strerror(errno));
        return;
    }

    xkb_keymap* keymap = xkb_keymap_new_from_string(
                self->_context, map_str,
                XKB_KEYMAP_FORMAT_TEXT_V1,
                XKB_KEYMAP_COMPILE_NO_FLAGS
            );
    munmap(map_str, size);
    close(fd);

    if (!keymap) {
        return;
    }
    self->_state = xkb_state_new(keymap);

    xkb_keymap_unref(keymap);

}
static void kbEnter(void* data, wl_keyboard* kb, uint32_t serial, wl_surface* surface,
        wl_array *keys) {
    auto self = reinterpret_cast<Keyboard*>(data);
    auto win = self->_wm.getWindowForNative(surface);
    if (!win) return;
    self->_serial = serial;
    self->_focus = win;
    if (self->_state) {
        // TODO: keys
    }
}
static void kbLeave(void* data, wl_keyboard* kb, uint32_t serial, wl_surface* surface) {
    auto self = reinterpret_cast<Keyboard*>(data);
    self->_serial = -1;
    self->_focus = nullptr;
}
static void kbKey(void* data, wl_keyboard* kb, uint32_t serial, uint32_t time,
                uint32_t key, uint32_t state) {
    auto self = reinterpret_cast<Keyboard*>(data);
    if (!self->_state || !self->_focus) return;

    const xkb_keysym_t *syms;

    if (xkb_state_key_get_syms(self->_state, key + 8, &syms) != 1)
        return;
    jwm::Key jwmKey = KeyWayland::fromNative(syms[0]);
    if (jwmKey != jwm::Key::UNDEFINED) {
        jwm::KeyLocation location = jwm::KeyLocation::DEFAULT;
        JNILocal<jobject> keyEvent(
            jwm::app.getJniEnv(),
            classes::EventKey::make(
                    jwm::app.getJniEnv(),
                    jwmKey,
                    state == WL_KEYBOARD_KEY_STATE_PRESSED,
                    KeyWayland::getModifiers(self->_state),
                    location
                )
            );
        self->_focus->dispatch(keyEvent.get());
    }
    self->_repeatingText = false;
    self->_repeating = false;
    if (state != WL_KEYBOARD_KEY_STATE_PRESSED) {
        self->_lastPress = std::chrono::time_point<std::chrono::steady_clock>();
        self->_nextRepeat = std::chrono::time_point<std::chrono::steady_clock>();
        return;
    }
    // ??? 
    self->_lastPress = std::chrono::steady_clock::now();
    self->_repeatKey = jwmKey;
    if (self->_repeatRate > 0) {
        self->_repeating = true;
        self->_nextRepeat = self->_lastPress + std::chrono::milliseconds(self->_repeatDelay);
    }
    self->_wm.notifyLoop();
    char textBuffer[0x40];
    int count = xkb_state_key_get_utf8(self->_state, key + 8, textBuffer, sizeof(textBuffer)-1);
    // ???
    if (count >= sizeof(textBuffer) - 1) {
        return;
    }
    if (count > 0) {
        // ignore sinful control symbols
        if (textBuffer[0] != 127 && textBuffer[0] > 0x1f) {
            JNIEnv* env = jwm::app.getJniEnv();

            jwm::StringUTF16 converted = reinterpret_cast<const char*>(textBuffer);
            self->_repeatText = converted;
            if (self->_repeatRate > 0)
                self->_repeatingText = true;
            jwm::JNILocal<jstring> jtext = converted.toJString(env);

            jwm::JNILocal<jobject> eventTextInput(env, classes::EventTextInput::make(env, jtext.get()));
            

            self->_focus->dispatch(eventTextInput.get());
        }
    }
} 
void kbModifiers(void* data, wl_keyboard* kb, uint32_t serial, uint32_t mods_depressed, 
            uint32_t mods_latched, uint32_t mods_locked, uint32_t group) {
    auto self = reinterpret_cast<Keyboard*>(data);
    if (!self->_state) return;
    xkb_state_update_mask(self->_state,
            mods_depressed, mods_latched, mods_locked,
            0, 0, group);
}
void kbRepeatInfo(void* data, wl_keyboard* kb, int32_t rate, int32_t delay) {
    auto self = reinterpret_cast<Keyboard*>(data);
    self->_repeatRate = rate;
    self->_repeatDelay = delay;
}

wl_keyboard_listener Keyboard::_keyboardListener = {
    .keymap = kbKeymap,
    .enter = kbEnter,
    .leave = kbLeave,
    .key = kbKey,
    .modifiers = kbModifiers,
    .repeat_info = kbRepeatInfo
};
Keyboard::Keyboard(wl_keyboard* kb, WindowManagerWayland* wm):
    _keyboard(kb),
    _wm(*wm)
{
    wl_keyboard_add_listener(kb, &_keyboardListener, this);
    _context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
}

Keyboard::~Keyboard()
{
    if (_keyboard)
        wl_keyboard_release(_keyboard);
    if (_context)
        xkb_context_unref(_context);
}
