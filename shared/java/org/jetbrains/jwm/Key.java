package org.jetbrains.jwm;

import lombok.*;
import java.util.*;
import org.jetbrains.annotations.*;

// must be kept in sync with Key.hh
public enum Key {
    UNDEFINED("Undefined"),

    // KeyModifiers
    CAPS_LOCK("Caps Lock"),
    SHIFT("Shift", KeyType.MODIFIER),
    CONTROL("Ctrl", KeyType.MODIFIER),
    ALT("Alt", KeyType.MODIFIER),
    WIN_LOGO("Windows", KeyType.MODIFIER),
    LINUX_META("Meta", KeyType.MODIFIER),
    LINUX_SUPER("Super", KeyType.MODIFIER),
    MAC_COMMAND("Command", KeyType.MODIFIER),
    MAC_OPTION("Option", KeyType.MODIFIER),
    MAC_FN("Function", KeyType.MODIFIER),

    // Rest of the keys
    ENTER("Enter", KeyType.WHITESPACE),
    BACKSPACE("Backspace"),
    TAB("Tab", KeyType.WHITESPACE),
    CANCEL("Cancel"),
    CLEAR("Clear"),
    PAUSE("Pause"),
    ESCAPE("Esc"),
    SPACE("Space", KeyType.WHITESPACE),
    PAGE_UP("Page Up", KeyType.NAVIGATION),
    PAGE_DOWN("Page Down", KeyType.NAVIGATION),
    END("End", KeyType.NAVIGATION),
    HOME("Home", KeyType.NAVIGATION),
    LEFT("Left", KeyType.ARROW | KeyType.NAVIGATION),
    UP("Up", KeyType.ARROW | KeyType.NAVIGATION),
    RIGHT("Right", KeyType.ARROW | KeyType.NAVIGATION),
    DOWN("Down", KeyType.ARROW | KeyType.NAVIGATION),
    COMMA("Comma"),
    MINUS("Minus"),
    PERIOD("Period"),
    SLASH("Slash"),
    DIGIT0("0", KeyType.DIGIT),
    DIGIT1("1", KeyType.DIGIT),
    DIGIT2("2", KeyType.DIGIT),
    DIGIT3("3", KeyType.DIGIT),
    DIGIT4("4", KeyType.DIGIT),
    DIGIT5("5", KeyType.DIGIT),
    DIGIT6("6", KeyType.DIGIT),
    DIGIT7("7", KeyType.DIGIT),
    DIGIT8("8", KeyType.DIGIT),
    DIGIT9("9", KeyType.DIGIT),
    SEMICOLON("Semicolon"),
    EQUALS("Equals"),
    A("A", KeyType.LETTER),
    B("B", KeyType.LETTER),
    C("C", KeyType.LETTER),
    D("D", KeyType.LETTER),
    E("E", KeyType.LETTER),
    F("F", KeyType.LETTER),
    G("G", KeyType.LETTER),
    H("H", KeyType.LETTER),
    I("I", KeyType.LETTER),
    J("J", KeyType.LETTER),
    K("K", KeyType.LETTER),
    L("L", KeyType.LETTER),
    M("M", KeyType.LETTER),
    N("N", KeyType.LETTER),
    O("O", KeyType.LETTER),
    P("P", KeyType.LETTER),
    Q("Q", KeyType.LETTER),
    R("R", KeyType.LETTER),
    S("S", KeyType.LETTER),
    T("T", KeyType.LETTER),
    U("U", KeyType.LETTER),
    V("V", KeyType.LETTER),
    W("W", KeyType.LETTER),
    X("X", KeyType.LETTER),
    Y("Y", KeyType.LETTER),
    Z("Z", KeyType.LETTER),
    OPEN_BRACKET("Open Bracket"),
    BACK_SLASH("Back Slash"),
    CLOSE_BRACKET("Close Bracket"),
    MULTIPLY("Multiply"),
    ADD("Add"),
    SEPARATOR("Separator"),
    DELETE("Delete"),
    NUM_LOCK("Num Lock"),
    SCROLL_LOCK("Scroll Lock"),
    F1("F1", KeyType.FUNCTION),
    F2("F2", KeyType.FUNCTION),
    F3("F3", KeyType.FUNCTION),
    F4("F4", KeyType.FUNCTION),
    F5("F5", KeyType.FUNCTION),
    F6("F6", KeyType.FUNCTION),
    F7("F7", KeyType.FUNCTION),
    F8("F8", KeyType.FUNCTION),
    F9("F9", KeyType.FUNCTION),
    F10("F10", KeyType.FUNCTION),
    F11("F11", KeyType.FUNCTION),
    F12("F12", KeyType.FUNCTION),
    F13("F13", KeyType.FUNCTION),
    F14("F14", KeyType.FUNCTION),
    F15("F15", KeyType.FUNCTION),
    F16("F16", KeyType.FUNCTION),
    F17("F17", KeyType.FUNCTION),
    F18("F18", KeyType.FUNCTION),
    F19("F19", KeyType.FUNCTION),
    F20("F20", KeyType.FUNCTION),
    F21("F21", KeyType.FUNCTION),
    F22("F22", KeyType.FUNCTION),
    F23("F23", KeyType.FUNCTION),
    F24("F24", KeyType.FUNCTION),
    PRINTSCREEN("Print Screen"),
    INSERT("Insert"),
    HELP("Help"),
    BACK_QUOTE("Back Quote"),
    QUOTE("Quote"),
    MENU("Menu"),
    KANA("Kana"),
    VOLUME_UP("Volume Up", KeyType.MEDIA),
    VOLUME_DOWN("Volume Down", KeyType.MEDIA),
    MUTE("Mute", KeyType.MEDIA);

    @ApiStatus.Internal @Getter public final String _name;
    @ApiStatus.Internal public final int _mask;

    @ApiStatus.Internal public static final Key[] _values = values();

    public static Key makeFromInt(int v) {
        return _values[v];
    }

    private Key(String name, int mask) {
        _name = name;
        _mask = mask;
    }

    private Key(String name) {
        this(name, 0);
    }

    public boolean isFunctionKey() {
        return (_mask & KeyType.FUNCTION) != 0;
    }

    /**
     * Navigation keys are arrow keys and Page Down, Page Up, Home, End
     * (including keypad keys)
     */
    public boolean isNavigationKey() {
        return (_mask & KeyType.NAVIGATION) != 0;
    }

    public boolean isArrowKey() {
        return (_mask & KeyType.ARROW) != 0;
    }

    public boolean isModifierKey() {
        return (_mask & KeyType.MODIFIER) != 0;
    }

    public boolean isLetterKey() {
        return (_mask & KeyType.LETTER) != 0;
    }

    /**
     * All Digit keys (including the keypad digits)
     */
    public boolean isDigitKey() {
        return (_mask & KeyType.DIGIT) != 0;
    }

    public boolean isKeypadKey() {
        return (_mask & KeyType.KEYPAD) != 0;
    }

    /**
     * Space, tab and enter
     */
    public boolean isWhitespaceKey() {
        return (_mask & KeyType.WHITESPACE) != 0;
    }

    /**
     * All multimedia keys (channel up/down, volume control, etc...)
     */
    public boolean isMediaKey() {
        return (_mask & KeyType.MEDIA) != 0;
    }

    @ApiStatus.Internal public static final Map<String, Key> _nameMap;
    static {
        _nameMap = new HashMap<String, Key>(Key.values().length);
        for (Key c: Key.values()) {
            _nameMap.put(c._name, c);
        }
    }

    /**
     * Parses textual representation of a key.
     * @param name Textual representation of the key
     * @return Key for the key with the given name, null if the string
     *                 is unknown
     */
    public static Key fromString(String name) {
        return _nameMap.get(name);
    }
}