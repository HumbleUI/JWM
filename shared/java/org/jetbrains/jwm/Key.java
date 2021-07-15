package org.jetbrains.jwm;

import lombok.*;
import java.util.*;
import org.jetbrains.annotations.*;

// adapted from https://github.com/openjdk/jfx/blob/0e7cf623d3cc6dc25b944ef739acaf2de27d125b/modules/javafx.graphics/src/main/java/javafx/scene/input/KeyCode.java
// must be kept in sync with Key.hh
public enum Key {
    ENTER("Enter", KeyType.WHITESPACE),
    BACK_SPACE("Backspace"),
    TAB("Tab", KeyType.WHITESPACE),
    CANCEL("Cancel"),
    CLEAR("Clear"),
    SHIFT("Shift", KeyType.MODIFIER),
    CONTROL("Ctrl", KeyType.MODIFIER),
    ALT("Alt", KeyType.MODIFIER),
    PAUSE("Pause"),
    CAPS("Caps Lock"),
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
    NUMPAD0("Numpad 0", KeyType.DIGIT | KeyType.KEYPAD),
    NUMPAD1("Numpad 1", KeyType.DIGIT | KeyType.KEYPAD),
    NUMPAD2("Numpad 2", KeyType.DIGIT | KeyType.KEYPAD),
    NUMPAD3("Numpad 3", KeyType.DIGIT | KeyType.KEYPAD),
    NUMPAD4("Numpad 4", KeyType.DIGIT | KeyType.KEYPAD),
    NUMPAD5("Numpad 5", KeyType.DIGIT | KeyType.KEYPAD),
    NUMPAD6("Numpad 6", KeyType.DIGIT | KeyType.KEYPAD),
    NUMPAD7("Numpad 7", KeyType.DIGIT | KeyType.KEYPAD),
    NUMPAD8("Numpad 8", KeyType.DIGIT | KeyType.KEYPAD),
    NUMPAD9("Numpad 9", KeyType.DIGIT | KeyType.KEYPAD),
    MULTIPLY("Multiply"),
    ADD("Add"),
    SEPARATOR("Separator"),
    SUBTRACT("Subtract"),
    DECIMAL("Decimal"),
    DIVIDE("Divide"),
    DEL("Delete"), /* ASCII:Integer DEL */
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
    META("Meta", KeyType.MODIFIER),
    BACK_QUOTE("Back Quote"),
    QUOTE("Quote"),
    KP_UP("Numpad Up", KeyType.ARROW | KeyType.NAVIGATION | KeyType.KEYPAD),
    KP_DOWN("Numpad Down", KeyType.ARROW | KeyType.NAVIGATION | KeyType.KEYPAD),
    KP_LEFT("Numpad Left", KeyType.ARROW | KeyType.NAVIGATION | KeyType.KEYPAD),
    KP_RIGHT("Numpad Right", KeyType.ARROW | KeyType.NAVIGATION | KeyType.KEYPAD),
    DEAD_GRAVE("Dead Grave"),
    DEAD_ACUTE("Dead Acute"),
    DEAD_CIRCUMFLEX("Dead Circumflex"),
    DEAD_TILDE("Dead Tilde"),
    DEAD_MACRON("Dead Macron"),
    DEAD_BREVE("Dead Breve"),
    DEAD_ABOVEDOT("Dead Abovedot"),
    DEAD_DIAERESIS("Dead Diaeresis"),
    DEAD_ABOVERING("Dead Abovering"),
    DEAD_DOUBLEACUTE("Dead Doubleacute"),
    DEAD_CARON("Dead Caron"),
    DEAD_CEDILLA("Dead Cedilla"),
    DEAD_OGONEK("Dead Ogonek"),
    DEAD_IOTA("Dead Iota"),
    DEAD_VOICED_SOUND("Dead Voiced Sound"),
    DEAD_SEMIVOICED_SOUND("Dead Semivoiced Sound"),
    AMPERSAND("Ampersand"),
    ASTERISK("Asterisk"),
    QUOTEDBL("Double Quote"),
    LESS("Less"),
    GREATER("Greater"),
    BRACELEFT("Left Brace"),
    BRACERIGHT("Right Brace"),

    AT("At"),
    COLON("Colon"),
    CIRCUMFLEX("Circumflex"),
    DOLLAR("Dollar"),
    EURO_SIGN("Euro Sign"),
    EXCLAMATION_MARK("Exclamation Mark"),
    INVERTED_EXCLAMATION_MARK("Inverted Exclamation Mark"),
    LEFT_PARENTHESIS("Left Parenthesis"),
    NUMBER_SIGN("Number Sign"),
    PLUS("Plus"),
    RIGHT_PARENTHESIS("Right Parenthesis"),
    UNDERSCORE("Underscore"),
    WINDOWS("Windows", KeyType.MODIFIER),
    CONTEXT_MENU("Context Menu"),

    FINAL("Final"),
    CONVERT("Convert"),
    NONCONVERT("Nonconvert"),
    ACCEPT("Accept"),
    MODECHANGE("Mode Change"),
    KANA("Kana"),
    KANJI("Kanji"),
    ALPHANUMERIC("Alphanumeric"),
    KATAKANA("Katakana"),
    HIRAGANA("Hiragana"),
    FULL_WIDTH("Full Width"),
    HALF_WIDTH("Half Width"),
    ROMAN_CHARACTERS("Roman Characters"),
    ALL_CANDIDATES("All Candidates"),
    PREVIOUS_CANDIDATE("Previous Candidate"),
    CODE_INPUT("Code Input"),
    JAPANESE_KATAKANA("Japanese Katakana"),
    JAPANESE_HIRAGANA("Japanese Hiragana"),
    JAPANESE_ROMAN("Japanese Roman"),
    KANA_LOCK("Kana Lock"),
    INPUT_METHOD_ON_OFF("Input Method On/Off"),
    CUT("Cut"),
    COPY("Copy"),
    PASTE("Paste"),
    UNDO("Undo"),
    AGAIN("Again"),
    FIND("Find"),
    PROPS("Properties"),
    STOP("Stop"),
    COMPOSE("Compose"),
    ALT_GRAPH("Alt Graph", KeyType.MODIFIER),
    BEGIN("Begin"),
    UNDEFINED("Undefined"),

    // Mobile and Embedded Specific Key Codes
    SOFTKEY_0("Softkey 0"),
    SOFTKEY_1("Softkey 1"),
    SOFTKEY_2("Softkey 2"),
    SOFTKEY_3("Softkey 3"),
    SOFTKEY_4("Softkey 4"),
    SOFTKEY_5("Softkey 5"),
    SOFTKEY_6("Softkey 6"),
    SOFTKEY_7("Softkey 7"),
    SOFTKEY_8("Softkey 8"),
    SOFTKEY_9("Softkey 9"),
    GAME_A("Game A"),
    GAME_B("Game B"),
    GAME_C("Game C"),
    GAME_D("Game D"),
    STAR("Star"),
    POUND("Pound"),
    POWER("Power"),
    INFO("Info"),
    COLORED_KEY_0("Colored Key 0"),
    COLORED_KEY_1("Colored Key 1"),
    COLORED_KEY_2("Colored Key 2"),
    COLORED_KEY_3("Colored Key 3"),
    EJECT_TOGGLE("Eject", KeyType.MEDIA),
    PLAY("Play", KeyType.MEDIA),
    RECORD("Record", KeyType.MEDIA),
    FAST_FWD("Fast Forward", KeyType.MEDIA),
    REWIND("Rewind", KeyType.MEDIA),
    TRACK_PREV("Previous Track", KeyType.MEDIA),
    TRACK_NEXT("Next Track", KeyType.MEDIA),
    CHANNEL_UP("Channel Up", KeyType.MEDIA),
    CHANNEL_DOWN("Channel Down", KeyType.MEDIA),
    VOLUME_UP("Volume Up", KeyType.MEDIA),
    VOLUME_DOWN("Volume Down", KeyType.MEDIA),
    MUTE("Mute", KeyType.MEDIA),
    COMMAND("Command", KeyType.MODIFIER),
    FUNCTION("Function", KeyType.MODIFIER),
    SHORTCUT("Shortcut");

    @ApiStatus.Internal @Getter public final String _name;
    @ApiStatus.Internal public final int _mask;

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