package io.github.humbleui.jwm;

public enum Platform {
    WINDOWS,
    X11,
    MACOS;

    public static final Platform CURRENT;
    static {
        String os = System.getProperty("os.name").toLowerCase();
        if (os.contains("mac") || os.contains("darwin"))
            CURRENT = MACOS;
        else if (os.contains("windows"))
            CURRENT = WINDOWS;
        else if (os.contains("nux") || os.contains("nix"))
            CURRENT = X11;
        else
            throw new RuntimeException("Unsupported platform: " + os);
    }
}