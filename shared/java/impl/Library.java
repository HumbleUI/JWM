package org.jetbrains.jwm.impl;

import java.io.*;
import java.net.*;
import java.nio.file.*;
import java.util.*;
import lombok.*;
import org.jetbrains.annotations.*;
import org.jetbrains.jwm.*;

public class Library {
    @ApiStatus.Internal
    public static volatile boolean _loaded = false;

    public static String readResource(String path) {
        URL url = Library.class.getResource(path);
        if (url == null)
            return null;
        try (InputStream is = url.openStream()) {
            byte[] bytes = is.readAllBytes();
            return new String(bytes).trim();
        } catch (IOException e) {
            return null;
        }
    }

    public static synchronized void load() {
        if (_loaded) return;

        String version = readResource("/jwm.version");
        File tempDir = new File(System.getProperty("java.io.tmpdir"), "jwm_" + (version == null ? "" + System.nanoTime() : version));
        String os = System.getProperty("os.name").toLowerCase();
        
        if (Platform.CURRENT == Platform.MACOS) {
            String file = "aarch64".equals(System.getProperty("os.arch")) ? "libjwm_arm64.dylib" : "libjwm_x64.dylib";
            File library = _extract("/", file, tempDir);
            System.load(library.getAbsolutePath());
        } else if (Platform.CURRENT == Platform.WINDOWS) {
            File library = _extract("/", "jwm.dll", tempDir);
            System.load(library.getAbsolutePath());
        } else if (Platform.CURRENT == Platform.X11) {
            File library = _extract("/", "libjwm_x64.so", tempDir);
            System.load(library.getAbsolutePath());
        }

        if (tempDir.exists() && version == null) {
            Runtime.getRuntime().addShutdownHook(new Thread(() -> {
                try {
                    Files.walk(tempDir.toPath())
                         .map(Path::toFile)
                         .sorted(Comparator.reverseOrder())
                         .forEach((f) -> {
                            f.delete();
                         });
                } catch (IOException ex) {
                    ex.printStackTrace();
                }
            }));
        }
        
        _loaded = true;
        _nAfterLoad();
    }

    @ApiStatus.Internal
    @SneakyThrows
    public static File _extract(String resourcePath, String fileName, File tempDir) {
        File file;
        URL url = Library.class.getResource(resourcePath + fileName);
        if (url == null) {
            file = new File(fileName);
            if (!file.exists())
                throw new IllegalArgumentException("Library file " + fileName + " not found in " + resourcePath);
        } else if (url.getProtocol() == "file") {
            file = new File(url.toURI());
        } else {
            file = new File(tempDir, fileName);
            if (!file.exists()) {
                if (!tempDir.exists())
                    tempDir.mkdirs();
                try (InputStream is = url.openStream()) {
                    Files.copy(is, file.toPath(), StandardCopyOption.REPLACE_EXISTING);
                }
            }
        }
        System.out.println("Loading " + file);
        return file;
    }

    @ApiStatus.Internal public static native void _nAfterLoad();
}
